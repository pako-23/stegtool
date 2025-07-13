#ifndef IMG_JPEG_H_INCLUDED
#define IMG_JPEG_H_INCLUDED

#include <stdio.h>

struct jpeg_img;

struct jpeg_img *jpeg_img_new(FILE * fp);

#endif
