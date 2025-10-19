#include <ctype.h>
#include <img/img.h>
#include <img/ppm.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct ppm_img_s {
    struct img_s super;
    int version;
    uint16_t maxval;
    uint16_t *data;
};

static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);
static int get_pixel(const struct img_s *img, size_t x, size_t y, int cmp);
static void set_pixel(struct img_s *img, size_t x, size_t y, int cmp,
                      int value);

const unsigned char ppm3_magic[2] = { 'P', '3' };
const unsigned char ppm6_magic[2] = { 'P', '6' };

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
    .get_pixel = get_pixel,
    .set_pixel = set_pixel,
};

struct ppm_img_s *ppm_img_new(FILE *fp)
{
    struct ppm_img_s *ppm;
    struct img_s *img;

    ppm = malloc(sizeof(struct ppm_img_s));
    if (ppm == NULL) {
        free(ppm);
        return NULL;
    }

    img = (struct img_s *)ppm;
    ppm->super.ops = &ops;

    if (img->ops->init(img, fp) < 0) {
        free(img);
        return NULL;
    }

    return ppm;
}

int is_ppm_img(FILE *fp)
{
    unsigned char magic[2];
    size_t nread;

    nread = fread(magic, 1, 2, fp);

    int ret = nread == 2 && (memcmp(ppm3_magic, magic, 2) == 0 ||
                             memcmp(ppm6_magic, magic, 2) == 0);

    rewind(fp);

    return ret;
}

static int read_header(struct ppm_img_s *ppm, FILE *fp)
{
    unsigned char magic[2];
    // State indicates if the width, height, and maxval have been read
    // 0 -> needs to read width
    // 1 -> needs to read height
    // 2 -> needs to read maxval
    int state = 0;
    int c;

    if (fread(magic, 1, 2, fp) != 2)
        return -1;

    ppm->version = magic[1] - '0';

    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c))
            continue;

        if (c == '#') {
            do
                c = fgetc(fp);
            while (c != EOF && c != '\n' && c != '\r');

            if (c == EOF)
                break;
        } else if (isdigit(c)) {
            int value = c - '0';

            while ((c = fgetc(fp)) != EOF && isdigit(c))
                value = 10 * value + c - '0';

            if (state == 0) {
                ppm->super.width = value;
                state = 1;
            } else if (state == 1) {
                ppm->super.height = value;
                state = 2;
            } else {
                ppm->maxval = value;
                break;
            }
        }
    }

    ppm->super.pixel_size = 3;

    return 0;
}

static int init(struct img_s *img, FILE *fp)
{
    struct ppm_img_s *ppm = (struct ppm_img_s *)img;
    size_t mapsz, row = 0, col = 0;
    int cmp = 0;
    uint16_t pixel;

    if (!is_ppm_img(fp))
        return -1;

    if (read_header(ppm, fp) < 0)
        return -1;

    mapsz = img_width(img) * img_height(img) * img_pixel_size(img);
    ppm->data = malloc(sizeof(uint16_t) * mapsz);
    if (ppm->data == NULL)
        return -1;

    for (size_t i = 0; i < mapsz; ++i) {
        if (ppm->version == 3) {
            if (fscanf(fp, "%hu", &pixel) != 1)
                goto error;
        } else if (ppm->maxval < 256) {
            int c = fgetc(fp);
            if (c == EOF)
                goto error;

            pixel = c;
        } else {
            if (fread(&pixel, 2, 1, fp) != 1)
                goto error;

            pixel = ntohs(pixel);
        }

        img->ops->set_pixel(img, row, col, cmp, pixel);
        if (++cmp == 3) {
            cmp = 0;
            if (++col == img_width(img)) {
                ++row;
                col = 0;
            }
        }
    }

    return 0;

error:
    free(ppm->data);
    return -1;
}

static void destroy(struct img_s *img)
{
    free(((struct ppm_img_s *)img)->data);
    free(img);
}

static int save(const struct img_s *img, FILE *fp)
{
    const struct ppm_img_s *ppm = (const struct ppm_img_s *)img;

    if (ppm->version == 3)
        fwrite(ppm3_magic, 1, 2, fp);
    else
        fwrite(ppm6_magic, 1, 2, fp);

    fputc('\n', fp);
    fprintf(fp, "%zu %zu\n", img_width(img), img_height(img));
    fprintf(fp, "%hu\n", ppm->maxval);

    if (ppm->version == 3) {
        for (size_t row = 0; row < img_height(img); ++row) {
            fprintf(fp, "%hu %hu %hu", img->ops->get_pixel(img, row, 0, 0),
                    img->ops->get_pixel(img, row, 0, 1),
                    img->ops->get_pixel(img, row, 0, 2));
            for (size_t col = 1; col < img_width(img); ++col)
                fprintf(fp, " %hu %hu %hu",
                        img->ops->get_pixel(img, row, col, 0),
                        img->ops->get_pixel(img, row, col, 1),
                        img->ops->get_pixel(img, row, col, 2));

            fputc('\n', fp);
        }
    } else {
        for (size_t row = 0; row < img_height(img); ++row) {
            uint16_t r, g, b;

            for (size_t col = 0; col < img_width(img); ++col) {
                r = img->ops->get_pixel(img, row, col, 0);
                g = img->ops->get_pixel(img, row, col, 1);
                b = img->ops->get_pixel(img, row, col, 2);

                if (ppm->maxval < 256) {
                    fputc(r, fp);
                    fputc(g, fp);
                    fputc(b, fp);
                } else {
                    fputc(htons(r), fp);
                    fputc(htons(g), fp);
                    fputc(htons(b), fp);
                }
            }
        }
    }

    return 0;
}

static int get_pixel(const struct img_s *img, size_t row, size_t col, int cmp)
{
    struct ppm_img_s *ppm = (struct ppm_img_s *)img;
    size_t stride = img_pixel_size(img) * img_width(img);
    uint16_t *p = ppm->data + stride * row + img_pixel_size(img) * col;

    return p[cmp];
}

static void set_pixel(struct img_s *img, size_t row, size_t col, int cmp,
                      int value)
{
    struct ppm_img_s *ppm = (struct ppm_img_s *)img;
    size_t stride = img_pixel_size(img) * img_width(img);
    uint16_t *p = ppm->data + stride * row + img_pixel_size(img) * col;

    p[cmp] = (uint16_t)value;
    if (p[cmp] > ppm->maxval)
        ppm->maxval = p[cmp];
}
