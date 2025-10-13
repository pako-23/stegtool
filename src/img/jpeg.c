#include <img/img.h>
#include <img/jpeg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <jpeglib.h>

struct jpeg_img_s {
    struct img_s super;
    J_COLOR_SPACE color_space;
    int precision;
    unsigned char *data;
};


static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);
static uint8_t *pixel(struct img_s *img, size_t x, size_t y);


const unsigned char jpeg_magic[3] = { 0xff, 0xd8, 0xff };

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
    .pixel = pixel,
};


struct jpeg_img_s *jpeg_img_new(FILE *fp)
{
    struct jpeg_img_s *jpeg;
    struct img_s *img;

    jpeg = malloc(sizeof(struct jpeg_img_s));
    if (jpeg == NULL) {
        free(jpeg);
        return NULL;
    }

    img = (struct img_s *)jpeg;

    jpeg->super.ops = &ops;

    if (jpeg->super.ops->init(img, fp) < 0) {
        free(img);
        return NULL;
    }

    return jpeg;
}

static void jpeg_error_output_message(j_common_ptr cinfo)
{
    cinfo->err->msg_code = -1;
}

static void jpeg_error_exit(j_common_ptr cinfo)
{
    cinfo->err->msg_code = -1;
}

static int jpeg_error_is_error(struct jpeg_error_mgr *err)
{
    return err->msg_code < 0 || err->num_warnings != 0;
}

static int init(struct img_s *img, FILE *fp)
{
    struct jpeg_img_s *jpgimg = (struct jpeg_img_s *)img;
    struct jpeg_decompress_struct info;
    int err = 0;
    struct jpeg_error_mgr jerr;
    size_t stride;
    unsigned char *buf;

    info.err = jpeg_std_error(&jerr);
    jerr.error_exit = jpeg_error_exit;
    jerr.output_message = jpeg_error_output_message;
    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, fp);
    if (jpeg_read_header(&info, TRUE) != JPEG_HEADER_OK ||
        jpeg_error_is_error(&jerr)) {
        err = -1;
        goto out;
    }
    jpeg_start_decompress(&info);

    img->width = info.image_width;
    img->height = info.output_height;
    img->pixel_size = info.num_components;
    jpgimg->color_space = info.out_color_space;
    jpgimg->precision = info.data_precision;

    stride = info.num_components * img->width;
    jpgimg->data = malloc(stride * img->height);
    if (jpgimg->data == NULL) {
        err = -1;
        goto out;
    }

    while (info.output_scanline < info.output_height) {
        buf = jpgimg->data + stride * info.output_scanline;
        jpeg_read_scanlines(&info, &buf, 1);
        if (jpeg_error_is_error(&jerr)) {
            err = -1;
            goto out;
        }
    }

    jpeg_finish_decompress(&info);
out:
    jpeg_destroy_decompress(&info);

    return err;
}

static void destroy(struct img_s *img)
{
    struct jpeg_img_s *jpgimg = (struct jpeg_img_s *)img;

    free(jpgimg->data);
    free(img);
}

static int save(const struct img_s *img, FILE *fp)
{
    struct jpeg_img_s *jpgimg = (struct jpeg_img_s *)img;
    struct jpeg_compress_struct info;
    struct jpeg_error_mgr jerr;
    size_t stride;
    unsigned char *buf;

    info.err = jpeg_std_error(&jerr);
    jerr.error_exit = jpeg_error_exit;
    jerr.output_message = jpeg_error_output_message;
    jpeg_create_compress(&info);
    jpeg_stdio_dest(&info, fp);

    info.image_width = img_width(img);
    info.image_height = img_height(img);
    info.input_components = img_pixel_size(img);
    info.in_color_space = jpgimg->color_space;
    info.data_precision = jpgimg->precision;

    jpeg_set_defaults(&info);
    jpeg_start_compress(&info, TRUE);

    stride = jpgimg->super.width * img_pixel_size(img);
    while (info.next_scanline < info.image_height) {
        buf = jpgimg->data + info.next_scanline * stride;
        jpeg_write_scanlines(&info, &buf, 1);
    }

    jpeg_finish_compress(&info);
    jpeg_destroy_compress(&info);
    return 0;
}

static uint8_t *pixel(struct img_s *img, size_t row, size_t col)
{
    struct jpeg_img_s *jpgimg = (struct jpeg_img_s *)img;
    size_t stride = img_width(img)*img_pixel_size(img);

    return jpgimg->data + row*stride + col*img_pixel_size(img);
}
