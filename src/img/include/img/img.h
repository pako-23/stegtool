#ifndef IMG_IMG_H_INCLUDED
#define IMG_IMG_H_INCLUDED

#include <stddef.h>
#include <stdio.h>

struct img_s;

struct img_ops_s {
    int (*init)(struct img_s *, FILE *);
    void (*destroy)(struct img_s *);
    int (*save)(const struct img_s *, FILE *);
};

struct img_s {
    const struct img_ops_s *ops;
    size_t width;
    size_t height;
};

struct img_s *img_from_file(const char *fname);
void img_destroy(struct img_s *img);

size_t img_width(const struct img_s *img);
size_t img_height(const struct img_s *img);
int img_save(const struct img_s *img, const char *fname);

#endif
