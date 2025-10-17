#ifndef IMG_PPM_H_INCLUDED
#define IMG_PPM_H_INCLUDED

#include <stdio.h>

struct ppm_img_s;

struct ppm_img_s *ppm_img_new(FILE *fp);
int is_ppm_img(FILE *fp);

#endif
