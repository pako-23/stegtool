#ifndef IMG_JPEG_H_INCLUDED
#define IMG_JPEG_H_INCLUDED

#include <stdio.h>

struct jpeg_img;

struct jpeg_img *jpeg_img_new(FILE *fp) {
    struct img_struct *img = malloc(sizeof(struct img_struct));
    img->width = 800;
    img->height = 600;
    img->ops = NULL;
    return img;
}

#endif
