#include <img/img.h>
#include <img/jpeg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct jpeg_img {
    struct img_s super;
};


const unsigned char jpeg_magic[4] = {0xff, 0xd8, 0xff, 0xe0};

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
    return NULL;
}


static int init(struct img_s *img, FILE *fp)
{
    return -1;
}

static void destroy(struct img_s *img) 
{}

static int save(const struct img_s *img, FILE *fp) {
    return -1;
}
