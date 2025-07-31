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

  EXPECT_THAT(img, Not(IsNull()));
  EXPECT_THAT(img_width(img), Eq(320));
  EXPECT_THAT(img_height(img), Eq(395));

  img_destroy(img);
}

TEST(PNGTest, ImgFromFileIvalidMagic) {
  struct img_s *img = img_from_file("invalid-magic.png");

  EXPECT_THAT(img, IsNull());
}

TEST(PNGTest, NewPNGImg) {
  FILE *fp;
  struct img_s *img;

  fp = fopen("cat.png", "r");
  EXPECT_THAT(fp, Not(IsNull()));

  img = (struct img_s *)png_img_new(fp);
  EXPECT_THAT(img, Not(IsNull()));
  EXPECT_THAT(img_width(img), Eq(320));
  EXPECT_THAT(img_height(img), Eq(395));

  img_destroy(img);
}

TEST(PNGTest, NewPNGImgInvalidMagic) {
  FILE *fp;
  struct img_s *img;

  fp = fopen("invalid-magic.png", "r");
  EXPECT_THAT(fp, Not(IsNull()));

  img = (struct img_s *)png_img_new(fp);
  EXPECT_THAT(img, IsNull());
  fclose(fp);
}

TEST(PNGTest, Save) {
  struct img_s *img = img_from_file("cat.png");

  EXPECT_THAT(img, Not(IsNull()));
  EXPECT_THAT(img_save(img, "cat-copy.png"), Not(Eq(0)));
  img_destroy(img);

  std::ifstream src("cat.png", std::ios::binary | std::ios::ate);
  std::ifstream dst("cat-copy.png", std::ios::binary | std::ios::ate);

  EXPECT_TRUE(src.is_open() && dst.is_open());

  const size_t buffer_size = 4096;
  std::vector<char> buf_src(buffer_size);
  std::vector<char> buf_dst(buffer_size);

  while (src.read(buf_src.data(), buffer_size) &&
         dst.read(buf_dst.data(), buffer_size))
    EXPECT_THAT(buf_src, Eq(buf_dst));

  EXPECT_TRUE(src.eof() && dst.eof());
}
