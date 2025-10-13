#include <img/img.h>
#include <img/png.h>
#include <img/jpeg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct img_it {
    struct img_s *img;
    size_t row;
    size_t col;
};


struct img_s *img_from_file(const char *fname)
{
    FILE *fp;
    unsigned char magic[8];
    struct img_s *img = NULL;
    size_t nread;
    size_t n;

    fp = fopen(fname, "rb");
    if (fp == NULL) {
        return NULL;
    }

    n = sizeof(jpeg_magic);
    nread = fread(magic, 1, n, fp);
    if (nread != n) {
        goto end;
    }

    if (memcmp(magic, jpeg_magic, n) == 0) {
        rewind(fp);
        img = (struct img_s *)jpeg_img_new(fp);
        goto end;
    }

    nread = fread(magic + n, 1, sizeof(png_magic) - n, fp);
    if (nread != sizeof(png_magic) - n) {
        goto end;
    }

    if (memcmp(magic, png_magic, sizeof(png_magic)) == 0) {
        rewind(fp);
        img = (struct img_s *)png_img_new(fp);
    }

end:
    fclose(fp);
    return img;
}

void img_destroy(struct img_s *img)
{
    img->ops->destroy(img);
}

size_t img_width(const struct img_s *img)
{
    return img->width;
}

size_t img_height(const struct img_s *img)
{
    return img->height;
}

int img_pixel_size(const struct img_s *img)
{
    return img->pixel_size;
}

int img_save(const struct img_s *img, const char *fname)
{
    FILE *fp;
    int ret;

    fp = fopen(fname, "wb");
    if (fp == NULL)
        return -1;

    ret = img->ops->save(img, fp);

    fclose(fp);
    return ret;
}

struct img_it *img_iterator(struct img_s *img)
{
    struct img_it *it;

    it = malloc(sizeof(struct img_it));
    if (it != NULL) {
        it->img = img;
        it->row = 0;
        it->col = 0;

    }

    return it;
}

void img_it_destroy(struct img_it *it)
{
    free(it);
}

void img_it_next(struct img_it *it)
{
    if (++it->col >= img_width(it->img)) {
        it->col = 0;
        ++it->row;
    }
}

int img_it_has_next(const struct img_it *it)
{
    return it->row < img_height(it->img);
}

uint8_t *img_it_deref(const struct img_it *it)
{
    return it->img->ops->pixel(it->img, it->row, it->col);
}
