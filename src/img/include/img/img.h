#ifndef IMG_IMG_H_INCLUDED
#define IMG_IMG_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

struct img_s;
struct img_it;

struct img_ops_s {
    int (*init)(struct img_s *, FILE *);
    void (*destroy)(struct img_s *);
    int (*save)(const struct img_s *, FILE *);
    uint8_t *(*pixel)(struct img_s *, size_t, size_t);
};

struct img_s {
    const struct img_ops_s *ops;
    size_t width;
    size_t height;
    int pixel_size;
};

struct img_s *img_from_file(const char *fname);
void img_destroy(struct img_s *img);

size_t img_width(const struct img_s *img);
size_t img_height(const struct img_s *img);
int img_pixel_size(const struct img_s *img);
int img_save(const struct img_s *img, const char *fname);

struct img_it *img_iterator(struct img_s *img);
void img_it_destroy(struct img_it *it);
void img_it_next(struct img_it *it);
int img_it_has_next(const struct img_it *it);
uint8_t *img_it_deref(const struct img_it *it);

#endif
