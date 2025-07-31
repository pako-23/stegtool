#ifndef IMG_JPEG_H_INCLUDED
#define IMG_JPEG_H_INCLUDED

#include <stdio.h>

struct jpeg_img_s;

extern const unsigned char jpeg_magic[4];

struct jpeg_img_s *jpeg_img_new(FILE * fp);

#endif
