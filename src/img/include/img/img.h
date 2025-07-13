#ifndef IMG_IMG_H_INCLUDED
#define IMG_IMG_H_INCLUDED

#include <stddef.h>
#include <stdio.h>

struct img_struct;

struct img_struct_ops {
    int (*init)(struct img_struct *, FILE * fp);
    void (*destroy)(struct img_struct *);
    int (*save)(struct img_struct *, FILE * fp);
};

struct img_struct {
    const struct img_struct_ops *ops;
    size_t width;
    size_t height;
};

struct img_struct *img_from_file(const char *fname);
void img_destroy(struct img_struct *img);

size_t img_width(const struct img_struct *img);
size_t img_height(const struct img_struct *img);
int img_save(struct img_struct *img, const char *fname);

#endif
