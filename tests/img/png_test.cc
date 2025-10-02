extern "C" {
#include <img/img.h>
#include <img/png.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

using namespace testing;

TEST(PNGTest, ImgFromFile)
{
    struct img_s *img = img_from_file("cat.png");

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(320));
    ASSERT_THAT(img_height(img), Eq(395));

    img_destroy(img);
}

TEST(PNGTest, ImgFromFileIvalidMagic)
{
    struct img_s *img = img_from_file("invalid-magic.png");

    ASSERT_THAT(img, IsNull());
}

TEST(PNGTest, NewPNGImg)
{
    FILE *fp;
    struct img_s *img;

    fp = fopen("cat.png", "rb");
    ASSERT_THAT(fp, Not(IsNull()));

    img = (struct img_s *)png_img_new(fp);
    fclose(fp);

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(320));
    ASSERT_THAT(img_height(img), Eq(395));

    img_destroy(img);
}

TEST(PNGTest, NewPNGImgInvalidMagic)
{
    FILE *fp;
    struct img_s *img;

    fp = fopen("invalid-magic.png", "rb");
    ASSERT_THAT(fp, Not(IsNull()));

    img = (struct img_s *)png_img_new(fp);
    ASSERT_THAT(img, IsNull());
    fclose(fp);
}

TEST(PNGTest, Save)
{
    struct img_s *img = img_from_file("cat.png");
    size_t width, height;

    ASSERT_THAT(img, Not(IsNull()));
    width = img_width(img);
    height = img_height(img);
    ASSERT_THAT(img_save(img, "cat-copy.png"), Eq(0));
    img_destroy(img);

    img = img_from_file("cat-copy.png");
    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(width));
    ASSERT_THAT(img_height(img), Eq(height));
    img_destroy(img);
}

TEST(PNGTest, PixelIteration)
{
    struct img_s *img = img_from_file("cat.png");
    ASSERT_THAT(img, Not(IsNull()));
    size_t expected_pixels = img_width(img) * img_height(img);

    struct img_it *it = img_iterator(img);
    ASSERT_THAT(it, Not(IsNull()));

    size_t pixels = 0;
    while (img_it_has_next(it)) {
        img_it_next(it);
        ++pixels;
    }
    img_it_destroy(it);

    ASSERT_THAT(pixels, Eq(expected_pixels));

    img_destroy(img);
}
