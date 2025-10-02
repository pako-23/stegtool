#include <img/img.h>
#include <img/jpeg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

struct segment {
    unsigned char marker[2];
    size_t len;
    struct segment *next;
    unsigned char data[];
};

struct jpeg_img_s {
    struct img_s super;
    struct segment *head;
    struct segment *tail;
};

const unsigned char jpeg_magic[3] = { 0xff, 0xd8, 0xff };

static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
};

struct jpeg_img_s *jpeg_img_new(FILE *fp)
{
    struct jpeg_img_s *jpeg;
    struct img_s *img;

    jpeg = malloc(sizeof(struct jpeg_img_s));
    if (jpeg == NULL) {
        free(jpeg);
        return NULL;
    }

    img = (struct img_s *)jpeg;

    jpeg->super.ops = &ops;
    jpeg->head = NULL;
    jpeg->tail = NULL;

    if (jpeg->super.ops->init(img, fp) < 0) {
        img_destroy(img);
        return NULL;
    }

    return jpeg;
}

static int init(struct img_s *img, FILE *fp)
{
    unsigned char marker[2];
    size_t len;
    struct jpeg_img_s *jpeg = (struct jpeg_img_s *)img;
    struct segment *s;

    while (1) {
        if (fread(marker, 1, 2, fp) != 2 || marker[0] != 0xff)
            return -1;

        if (marker[1] >= 0xd0 && marker[1] <= 0xd9) {
            len = 0;
        } else if (marker[1] == 0xc0 || marker[1] == 0xc2 ||
                   marker[1] == 0xc4 || marker[1] == 0xdb ||
                   marker[1] == 0xdd || marker[1] == 0xfe ||
                   (marker[1] >> 4) == 0xe) {
            if (fread(&len, 1, 2, fp) != 2)
                return -1;
            len = ntohs(len) - 2;
        } else if (marker[1] == 0xda) {
            long pos = ftell(fp);
            if (pos < 0)
                return -1;

            if (fseek(fp, -2, SEEK_END) != 0)
                return -1;

            long size = ftell(fp);
            if (size == -1)
                return -1;

            if (fseek(fp, pos, SEEK_SET) != 0)
                return -1;

            len = size - pos;
        } else {
            return -1;
        }

        s = malloc(sizeof(struct segment) + len);
        if (s == NULL)
            return -1;

        if (len > 0 && fread(s->data, 1, len, fp) != len) {
            free(s);
            return -1;
        }

        memcpy(s->marker, marker, 2);
        s->len = len;

        if (marker[1] == 0xc0 || marker[1] == 0xc2) {
            jpeg->super.height = ntohs(*((uint16_t *)(s->data + 1)));
            jpeg->super.width = ntohs(*((uint16_t *)(s->data + 3)));
        }

        s->next = NULL;
        if (jpeg->head == NULL)
            jpeg->head = s;
        else
            jpeg->tail->next = s;
        jpeg->tail = s;

        if (marker[1] == 0xd9)
            break;
    }

    return 0;
}

static void destroy(struct img_s *img)
{
    struct segment *s;
    struct jpeg_img_s *jpeg = (struct jpeg_img_s *)img;

    while (jpeg->head != NULL) {
        s = jpeg->head;
        jpeg->head = s->next;
        free(s);
    }

    free(jpeg);
}

static int save(const struct img_s *img, FILE *fp)
{
    const struct jpeg_img_s *jpeg = (const struct jpeg_img_s *)img;
    struct segment *s;

    for (s = jpeg->head; s != NULL; s = s->next) {
        if (fwrite(s->marker, 1, 2, fp) != 2)
            return -1;

        if (s->marker[1] >= 0xd0 && s->marker[1] <= 0xd9) {
            continue;
        } else if (s->marker[1] == 0xda) {
            if (fwrite(s->data, 1, s->len, fp) != s->len)
                return -1;
        } else {
            uint16_t len = htons(s->len + 2);

            if (fwrite(&len, 1, 2, fp) != 2 ||
                fwrite(s->data, 1, s->len, fp) != s->len)
                return -1;
        }
    }

    return 0;
}
