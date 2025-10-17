#ifndef IMG_PNG_H_INCLUDED
#define IMG_PNG_H_INCLUDED

#include <stdio.h>

struct png_img_s;

struct png_img_s *png_img_new(FILE *fp);
int is_png_img(FILE *fp);

#endif
