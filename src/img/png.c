#include <img/img.h>
#include <img/png.h>


struct png_img {
    struct img_s super;
};

static int init(struct img_s *img, FILE *fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE *fp);

const unsigned char png_magic[4] = {0x89, 0x50, 0x4e, 0x47};

static const struct img_ops_s ops = {
    .init = init,
    .destroy = destroy,
    .save = save,
};


struct png_img_s *png_img_new(FILE *fp)
{
    return NULL;
}

static int init(struct img_s *img, FILE *fp)
{
    return -1;
}

static void destroy(struct img_s *img)
{
}

static int save(const struct img_s *img, FILE *fp)
{
    return -1;
}
