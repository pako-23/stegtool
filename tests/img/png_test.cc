extern "C" {
#include <img/img.h>
#include <img/png.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

using namespace testing;

TEST(PNGTest, ImgFromFile) {
  struct img_s *img = img_from_file("cat.png");

  ASSERT_THAT(img, Not(IsNull()));
  ASSERT_THAT(img_width(img), Eq(320));
  ASSERT_THAT(img_height(img), Eq(395));

  img_destroy(img);
}

TEST(PNGTest, ImgFromFileIvalidMagic) {
  struct img_s *img = img_from_file("invalid-magic.png");

  ASSERT_THAT(img, IsNull());
}

TEST(PNGTest, NewPNGImg) {
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

TEST(PNGTest, NewPNGImgInvalidMagic) {
  FILE *fp;
  struct img_s *img;

  fp = fopen("invalid-magic.png", "rb");
  ASSERT_THAT(fp, Not(IsNull()));

  img = (struct img_s *)png_img_new(fp);
  ASSERT_THAT(img, IsNull());
  fclose(fp);
}

TEST(PNGTest, Save) {
  struct img_s *img = img_from_file("cat.png");

  ASSERT_THAT(img, Not(IsNull()));
  ASSERT_THAT(img_save(img, "cat-copy.png"), Eq(0));
  img_destroy(img);

  std::ifstream src("cat.png", std::ios::binary | std::ios::in);
  std::ifstream dst("cat-copy.png", std::ios::binary | std::ios::in);

  ASSERT_TRUE(src.is_open() && dst.is_open());

  const size_t buffer_size = 4096;
  std::vector<char> buf_src(buffer_size);
  std::vector<char> buf_dst(buffer_size);

  while (src.read(buf_src.data(), buffer_size) &&
         dst.read(buf_dst.data(), buffer_size))
    ASSERT_THAT(buf_src, Eq(buf_dst));

  ASSERT_FALSE(src.read(buf_src.data(), buffer_size));
  ASSERT_FALSE(dst.read(buf_dst.data(), buffer_size));
  ASSERT_TRUE(src.eof());
  ASSERT_TRUE(dst.eof());
}

TEST(PNGTest, PixelIteration) {
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

  ASSERT_THAT(pixels, Eq(expected_pixels));

  img_destroy(img);
}
