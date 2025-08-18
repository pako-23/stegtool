#ifndef IMG_PNG_H_INCLUDED
#define IMG_PNG_H_INCLUDED

#include <stdio.h>

struct png_img_s;

extern const unsigned char png_magic[8];

struct png_img_s *png_img_new(FILE * fp);

#endif
