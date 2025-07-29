#ifndef IMG_JPEG_H_INCLUDED
#define IMG_JPEG_H_INCLUDED

#include <stdio.h>
#include "img.h"

struct png_img {
    struct img_struct base;
};

struct png_img *png_img_new(FILE *fp);
int png_img_init(struct img_struct *img, FILE *fp);

#endif