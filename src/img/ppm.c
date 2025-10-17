#include <ctype.h>
#include <img/img.h>
#include <img/ppm.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <jpeglib.h>


struct ppm_img_s {
    struct img_s super;
    unsigned char *data;
    int version;
};

static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);
static uint8_t *pixel(struct img_s *img, size_t x, size_t y);

const unsigned char ppm3_magic[2] = { 'P', '3' };
const unsigned char ppm6_magic[2] = { 'P', '6' };

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
    .pixel = pixel,
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
    const size_t n = 2;
    unsigned char magic[n];
    size_t nread;

    nread = fread(magic, 1, n, fp);

    int ret = nread == n && (memcmp(ppm3_magic, magic, n) == 0
                             || memcmp(ppm6_magic, magic, n) == 0);

    rewind(fp);

    return ret;
}

static int init(struct img_s *img, FILE *fp)
{
    struct ppm_img_s *ppm = (struct ppm_img_s *)img;
    unsigned char magic[2];
    int c;

    // State indicates if the width, height, and maxval have been read
    // 0 -> needs to read width
    // 1 -> needs to read height
    // 2 -> needs to read maxval
    // 3 -> has finished to read the
    int state = 0;

    if (!is_ppm_img(fp))
        return -1;

    if (fread(magic, 1, 2, fp) != 2) return -1;
    ppm->version = magic[1] - '0';


    while ((c = getchar()) != EOF) {
        if (isspace(c))
            continue;

        if (c == '#') {
            do
                c = getchar();
            while (c != EOF && c != '\n' && c != '\r');
            
            if (c == EOF) break;
        } else if (isdigit(c)) {
            size_t value = c - '0';

            while ((c = getchar()) != EOF && isdigit(c))
                value = 10 * value + c - '0';

            if (state == 0) {
                ppm->super.width = value;
                state = 1;
            } 
            
            // TODO read ascii number
        }
    }
    
    return 0;
}

static void destroy(struct img_s *img)
{
    free(img);
}

static int save(const struct img_s *img, FILE *fp)
{
    return 0;
}

static uint8_t *pixel(struct img_s *img, size_t row, size_t col)
{
    return NULL;
}
