#include <img/img.h>
#include <img/jpeg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct jpeg_img {
    struct img_struct super;
};

static int init(struct img_struct *img, FILE *fp);
static void destroy(struct img_struct *img);
static int save(struct img_struct *img, FILE *fp);


static const struct img_struct_ops ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
};

struct jpeg_img *jpeg_img_new(FILE *fp)
{
    struct jpeg_img *jpeg = malloc(sizeof(struct jpeg_img));
    if (!jpeg) return NULL;

    jpeg->super.ops = &ops;

    if (jpeg->super.ops->init((struct img_struct *)jpeg, fp) != 0) {
        free(jpeg);
        return NULL;
    }

    return jpeg;
}


static int init(struct img_struct *img, FILE *fp)
{
    unsigned char marker[2];
    if (fread(marker, 1, 2, fp) != 2)
        return -1;
    if (marker[0] != 0xFF || marker[1] != 0xD8)  // SOI check
        return -1;

    while (1) {
        if (fread(marker, 1, 2, fp) != 2)
            return -1;
        if (marker[0] != 0xFF)
            return -1;

        // SOF markers that contain frame info (skip some that are not SOF)
        if ((marker[1] >= 0xC0 && marker[1] <= 0xCF) &&
            marker[1] != 0xC4 && marker[1] != 0xC8 && marker[1] != 0xCC) {

            unsigned char length_bytes[2];
            if (fread(length_bytes, 1, 2, fp) != 2)
                return -1;
            uint16_t length = (length_bytes[0] << 8) | length_bytes[1];

            unsigned char *buf = malloc(length - 2);
            if (!buf)
                return -1;
            if (fread(buf, 1, length - 2, fp) != length - 2) {
                free(buf);
                return -1;
            }

            uint16_t height = (buf[1] << 8) | buf[2];
            uint16_t width = (buf[3] << 8) | buf[4];

            img->width = width;
            img->height = height;

            free(buf);
            return 0;
        } else {
            unsigned char length_bytes[2];
            if (fread(length_bytes, 1, 2, fp) != 2)
                return -1;
            uint16_t length = (length_bytes[0] << 8) | length_bytes[1];

            if (fseek(fp, length - 2, SEEK_CUR) != 0)
                return -1;
        }
    }
    return -1;
}

static void destroy(struct img_struct *img) 
{
    if (!img) return;

    struct jpeg_img *jpeg = (struct jpeg_img *)img;

    free(jpeg);
}

static int save(struct img_struct *img, FILE *fp)
{
    (void)img; 
    (void)fp;
    return -1;
}
