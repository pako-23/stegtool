#include <img/img.h>
#include <img/png.h>
#include <png.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct png_img_s {
    struct img_s super;
    png_bytepp rows;
};

static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);
static uint8_t *pixel(struct img_s *img, size_t x, size_t y);

const unsigned char png_magic[8] = { 0x89, 0x50, 0x4e, 0x47,
                                     0x0d, 0x0a, 0x1a, 0x0a };

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
    .pixel = pixel,
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

int is_png_img(FILE *fp)
{
    const size_t n = sizeof(png_magic);
    unsigned char magic[n];
    size_t nread;

    nread = fread(magic, 1, n, fp);

    int ret = nread == n && memcmp(png_magic, magic, n) == 0;

    rewind(fp);

    return ret;
}

static int init(struct img_s *img, FILE *fp)
{
    struct png_img_s *pngimg = (struct png_img_s *)img;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png;
    png_infop info;
    size_t row;

    if (!is_png_img(fp))
        return -1;

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
        return -1;

    info = png_create_info_struct(png);
    if (info == NULL) {
        png_destroy_read_struct(&png, NULL, NULL);
        return -1;
    }

    pngimg->rows = NULL;
    if (setjmp(png_jmpbuf(png)))
        goto error;

    png_init_io(png, fp);
    png_read_info(png, info);

    img->height = png_get_image_height(png, info);
    img->width = png_get_image_width(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);
    img->pixel_size = 4;

    pngimg->rows = (png_bytep *)malloc(sizeof(png_bytep) * img->height);
    for (row = 0; row < img->height; ++row)
        pngimg->rows[row] = (png_byte *)malloc(png_get_rowbytes(png, info));

    png_read_image(png, pngimg->rows);

    png_destroy_read_struct(&png, &info, NULL);

    return 0;

error:
    png_destroy_read_struct(&png, &info, NULL);

    if (pngimg->rows == NULL)
        return -1;

    for (size_t i = 0; i < row; ++i)
        free(pngimg->rows[i]);
    free((void *)pngimg->rows);

    return -1;
}

static void destroy(struct img_s *img)
{
    struct png_img_s *pngimg = (struct png_img_s *)img;

    for (size_t i = 0; i < img->height; ++i)
        free(pngimg->rows[i]);
    free((void *)pngimg->rows);

    free(img);
}

static int save(const struct img_s *img, FILE *fp)
{
    png_structp png;
    png_infop info;
    struct png_img_s *pngimg = (struct png_img_s *)img;

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
        return -1;

    info = png_create_info_struct(png);
    if (info == NULL) {
        png_destroy_write_struct(&png, NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        return -1;
    }

    png_init_io(png, fp);

    png_set_IHDR(png, info, img->width, img->height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);
    png_write_image(png, pngimg->rows);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);

    return 0;
}

static uint8_t *pixel(struct img_s *img, size_t row, size_t col)
{
    struct png_img_s *pngimg = (struct png_img_s *)img;

    return pngimg->rows[row] + col * img_pixel_size(img);
}
