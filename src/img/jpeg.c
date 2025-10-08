#include <img/img.h>
#include <img/jpeg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <jpeglib.h>

struct jpeg_img_s {
    struct img_s super;
    int pixelsz;
    J_COLOR_SPACE color_space;
    int precision;
    unsigned char *data;
};

struct jpeg_img_it {
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

const unsigned char jpeg_magic[3] = { 0xff, 0xd8, 0xff };

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
    jpgimg->pixelsz = info.num_components;
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

    info.image_width = jpgimg->super.width;
    info.image_height = jpgimg->super.height;
    info.input_components = jpgimg->pixelsz;
    info.in_color_space = jpgimg->color_space;
    info.data_precision = jpgimg->precision;

    jpeg_set_defaults(&info);
    jpeg_start_compress(&info, TRUE);

    stride = jpgimg->super.width * jpgimg->pixelsz;
    while (info.next_scanline < info.image_height) {
        buf = jpgimg->data + info.next_scanline * stride;
        jpeg_write_scanlines(&info, &buf, 1);
    }

    jpeg_finish_compress(&info);
    jpeg_destroy_compress(&info);
    return 0;
}

static struct img_it *iterator(struct img_s *img)
{
    struct jpeg_img_it *it;

    it = malloc(sizeof(struct jpeg_img_it));
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
    struct jpeg_img_it *jit = (struct jpeg_img_it *)it;

    if (++jit->col >= jit->width) {
        jit->col = 0;
        ++jit->row;
    }
}

static int has_next(const struct img_it *it)
{
    struct jpeg_img_it *jit = (struct jpeg_img_it *)it;

    return jit->row < jit->height;
}
