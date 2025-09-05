#ifndef IMG_IMG_H_INCLUDED
#define IMG_IMG_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

struct pixel_s {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct img_it;

struct img_it_ops {
	void (*destroy)(struct img_it *);
	void (*next)(struct img_it *);
	int (*has_next)(const struct img_it *);
	void (*read)(const struct img_it *, struct pixel_s *);
	void (*write)(const struct img_it *, const struct pixel_s *);
};

struct img_it {
	const struct img_it_ops *ops;
};

struct img_s;

struct img_ops_s {
	int (*init)(struct img_s *, FILE *);
	void (*destroy)(struct img_s *);
	int (*save)(const struct img_s *, FILE *);
	struct img_it *(*iterator) (struct img_s *);
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

struct img_it *img_iterator(struct img_s *img);

void img_it_destroy(struct img_it *it);
void img_it_next(struct img_it *it);
int img_it_has_next(const struct img_it *it);
void img_it_read(const struct img_it *it, struct pixel_s *pixel);
void img_it_write(const struct img_it *it, const struct pixel_s *pixel);

#endif
