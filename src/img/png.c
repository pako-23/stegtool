#include <img/img.h>
#include <img/png.h>
#include <png.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct png_img_s {
    struct img_s super;
    png_structp read;
    png_infop info;
    png_bytepp rows;
};

struct png_img_it {
    struct img_it super;
    size_t width;
    size_t height;
    size_t row;
    size_t col;
};

static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);
static struct img_it *iterator(struct img_s *img);

static void it_destroy(struct img_it *it);
static void next(struct img_it *it);
static int has_next(const struct img_it *it);
static void read(const struct img_it *it, struct pixel_s *p);
static void write(const struct img_it *it, const struct pixel_s *p);

const unsigned char png_magic[8] = { 0x89, 0x50, 0x4e, 0x47,
                                     0x0d, 0x0a, 0x1a, 0x0a };

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
    .iterator = iterator,
};

static const struct img_it_ops it_ops = {
    .destroy = it_destroy,
    .next = next,
    .has_next = has_next,
    .read = read,
    .write = write,
};

struct png_img_s *png_img_new(FILE *fp)
{
    struct png_img_s *png;
    struct img_s *img;

    png = malloc(sizeof(struct png_img_s));
    if (png == NULL)
        return NULL;

    img = (struct img_s *)png;
    png->super.ops = &ops;

    if (png->super.ops->init(img, fp) < 0) {
        free(img);
        return NULL;
    }

    return png;
}

static int init(struct img_s *img, FILE *fp)
{
    unsigned char header[8];
    struct png_img_s *pngimg = (struct png_img_s *)img;

    if (fread(header, 1, 8, fp) != 8 ||
        memcmp(header, png_magic, sizeof(header)) != 0)
        return -1;

    rewind(fp);
    pngimg->read =
            png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pngimg->read == NULL)
        return -1;

    pngimg->info = png_create_info_struct(pngimg->read);
    if (pngimg->info == NULL) {
        png_destroy_read_struct(&pngimg->read, NULL, NULL);
        return -1;
    }

    png_init_io(pngimg->read, fp);
    png_read_png(pngimg->read, pngimg->info, PNG_TRANSFORM_IDENTITY, NULL);
    img->height = png_get_image_height(pngimg->read, pngimg->info);
    img->width = png_get_image_width(pngimg->read, pngimg->info);
    pngimg->rows = png_get_rows(pngimg->read, pngimg->info);

    return 0;
}

static void destroy(struct img_s *img)
{
    struct png_img_s *pngimg = (struct png_img_s *)img;

    png_destroy_read_struct(&pngimg->read, &pngimg->info, NULL);
    free(img);
}

static int save(const struct img_s *img, FILE *fp)
{
    png_structp png;
    struct png_img_s *pngimg = (struct png_img_s *)img;

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
        return -1;

    png_init_io(png, fp);
    png_set_rows(png, pngimg->info, pngimg->rows);
    png_write_png(png, pngimg->info, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png, NULL);

    return 0;
}

static struct img_it *iterator(struct img_s *img)
{
    struct png_img_it *it;

    it = malloc(sizeof(struct png_img_it));
    if (it == NULL)
        return NULL;

    it->super.ops = &it_ops;
    it->height = img_height(img);
    it->width = img_width(img);
    it->col = 0;
    it->row = 0;

    return &it->super;
}

static void it_destroy(struct img_it *it)
{
    free(it);
}

static void next(struct img_it *it)
{
    struct png_img_it *pit = (struct png_img_it *)it;

    if (++pit->col >= pit->width) {
        pit->col = 0;
        ++pit->row;
    }
}

static int has_next(const struct img_it *it)
{
    struct png_img_it *pit = (struct png_img_it *)it;

    return pit->row < pit->height;
}

static void read(const struct img_it *it, struct pixel_s *p)
{
}

static void write(const struct img_it *it, const struct pixel_s *p)
{
}
