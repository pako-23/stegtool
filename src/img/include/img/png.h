#ifndef IMG_PNG_H_INCLUDED
#define IMG_PNG_H_INCLUDED

#include <stdio.h>

struct png_img;

struct png_img *png_img_new(FILE *fp) {
    struct img_struct *img = malloc(sizeof(struct img_struct));
    img->width = 1024;
    img->height = 768;
    img->ops = NULL; //whats ops??
    return img;
}

#endif
