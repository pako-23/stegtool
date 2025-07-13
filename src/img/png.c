#include <img/img.h>
#include <img/png.h>

struct png_img {
    struct img_struct super;
};

static int init(struct img_struct *img, FILE *fp);
static void destroy(struct img_struct *img);
static int save(struct img_struct *img, FILE *fp);

static const struct img_struct_ops ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
};

struct png_img *png_img_new(FILE *fp)
{
    return NULL;
}

static int init(struct img_struct *img, FILE *fp)
{
    return 0;
}

static void destroy(struct img_struct *img) {}

static int save(struct img_struct *img, FILE *fp)
{
    return 0;
}
