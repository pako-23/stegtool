extern "C" {
#include <img/img.h>
#include <img/jpeg.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

using namespace testing;

TEST(JPEGTest, ImgFromFile) {
  struct img_s *img = img_from_file("cat.jpg");

  ASSERT_THAT(img, Not(IsNull()));
  ASSERT_THAT(img_width(img), Eq(539));
  ASSERT_THAT(img_height(img), Eq(360));

  img_destroy(img);
}

TEST(JPEGTest, ImgFromFileIvalidMagic) {
  struct img_s *img = img_from_file("invalid-magic.jpg");

  ASSERT_THAT(img, IsNull());
}

TEST(JPEGTest, NewJPEGImg) {
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

TEST(JPEGTest, NewJPEGImgInvalidMagic) {
  FILE *fp;
  struct img_s *img;

  fp = fopen("invalid-magic.jpg", "rb");
  EXPECT_THAT(fp, Not(IsNull()));

  img = (struct img_s *)jpeg_img_new(fp);
  EXPECT_THAT(img, IsNull());
  fclose(fp);
}

TEST(JPEGTest, Save) {
  struct img_s *img = img_from_file("cat.jpg");

  ASSERT_THAT(img, Not(IsNull()));
  ASSERT_THAT(img_save(img, "cat-copy.jpg"), Eq(0));
  img_destroy(img);

  std::ifstream src("cat.jpg", std::ios::binary | std::ios::in);
  std::ifstream dst("cat-copy.jpg", std::ios::binary | std::ios::in);

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
