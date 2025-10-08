extern "C" {
#include <img/img.h>
#include <img/jpeg.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

TEST(JPEGTest, ImgFromFile)
{
    struct img_s *img = img_from_file("cat.jpg");

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(539));
    ASSERT_THAT(img_height(img), Eq(360));

    img_destroy(img);
}

TEST(JPEGTest, ImgFromFileIvalidMagic)
{
    struct img_s *img = img_from_file("invalid-magic.jpg");

    ASSERT_THAT(img, IsNull());
}

TEST(JPEGTest, NewJPEGImg)
{
    FILE *fp;
    struct img_s *img;

    fp = fopen("cat.jpg", "rb");
    ASSERT_THAT(fp, Not(IsNull()));

    img = (struct img_s *)jpeg_img_new(fp);
    fclose(fp);

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(539));
    ASSERT_THAT(img_height(img), Eq(360));

    img_destroy(img);
}

TEST(JPEGTest, NewJPEGImgInvalidMagic)
{
    FILE *fp;
    struct img_s *img;

    fp = fopen("invalid-magic.jpg", "rb");
    EXPECT_THAT(fp, Not(IsNull()));

    img = (struct img_s *)jpeg_img_new(fp);
    EXPECT_THAT(img, IsNull());
    fclose(fp);
}

TEST(JPEGTest, Save)
{
    size_t width, height;
    struct img_s *img = img_from_file("cat.jpg");

    ASSERT_THAT(img, Not(IsNull()));
    width = img_width(img);
    height = img_height(img);
    ASSERT_THAT(img_save(img, "cat-copy.jpg"), Eq(0));
    img_destroy(img);

    img = img_from_file("cat-copy.jpg");
    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(width));
    ASSERT_THAT(img_height(img), Eq(height));
    img_destroy(img);
}

TEST(JPEGTest, PixelIteration)
{
    struct img_s *img = img_from_file("cat.jpg");
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
