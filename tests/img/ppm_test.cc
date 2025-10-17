extern "C" {
#include <img/img.h>
#include <img/ppm.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

class PPMTestImg {
public:
    PPMTestImg(const char *file)
            : file_{ file }
            , width_{ 0 }
            , height_{ 0 }
    {
        // TODO read image size
    }

    inline const char *file(void) const
    {
        return file_;
    }

    inline size_t width(void) const
    {
        return width_;
    }

    inline size_t height(void) const
    {
        return height_;
    }

private:
    const char *file_;
    size_t width_;
    size_t height_;
};

class PPMTest : public TestWithParam<PPMTestImg> {};

TEST(PPMTest, ImgFromFileIvalidMagic)
{
    struct img_s *img = img_from_file("invalid-magic.ppm");

    ASSERT_THAT(img, IsNull());
}

TEST(PPMTest, NewPPMImgInvalidMagic)
{
    FILE *fp;
    struct img_s *img;

    fp = fopen("invalid-magic.ppm", "rb");
    EXPECT_THAT(fp, NotNull());

    img = (struct img_s *)ppm_img_new(fp);
    EXPECT_THAT(img, IsNull());
    fclose(fp);
}

// TEST_P(PPMTest, ImgFromFile)
// {
//     PPMTestImg param = GetParam();
//     struct img_s *img = img_from_file(param.file());

//     ASSERT_THAT(img, Not(IsNull()));
//     ASSERT_THAT(img_width(img), Eq(param.width()));
//     ASSERT_THAT(img_height(img), Eq(param.height()));

//     img_destroy(img);
// }

// TEST_P(PPMTest, NewPPMImg)
// {
//     FILE *fp;
//     struct img_s *img;
//     PPMTestImg param = GetParam();

//     fp = fopen(param.file(), "rb");
//     ASSERT_THAT(fp, Not(IsNull()));

//     img = (struct img_s *)ppm_img_new(fp);
//     fclose(fp);

//     ASSERT_THAT(img, Not(IsNull()));
//     ASSERT_THAT(img_width(img), Eq(param.width()));
//     ASSERT_THAT(img_height(img), Eq(param.height()));

//     img_destroy(img);
// }

// TEST_P(PPMTest, PixelIteration)
// {
//     PPMTestImg param = GetParam();
//     struct img_s *img = img_from_file(param.file());
//     ASSERT_THAT(img, Not(IsNull()));
//     size_t expected_pixels = img_width(img) * img_height(img);

//     struct img_it *it = img_iterator(img);
//     ASSERT_THAT(it, Not(IsNull()));

//     size_t pixels = 0;
//     while (img_it_has_next(it)) {
//         img_it_next(it);
//         ++pixels;
//     }
//     img_it_destroy(it);

//     ASSERT_THAT(pixels, Eq(expected_pixels));

//     img_destroy(img);
// }

// TEST_P(PPMTest, Save)
// {
//     PPMTestImg param = GetParam();
//     struct img_s *img = img_from_file(param.file());
//     struct img_s *simg;
//     std::string saved = "saved-" + std::string{ param.file() };
//     int pxlsz;

//     ASSERT_THAT(img, Not(IsNull()));
//     ASSERT_THAT(img_save(img, saved.c_str()), Eq(0));

//     simg = img_from_file(saved.c_str());
//     ASSERT_THAT(simg, Not(IsNull()));
//     ASSERT_THAT(img_width(simg), Eq(img_width(img)));
//     ASSERT_THAT(img_height(simg), Eq(img_height(img)));
//     ASSERT_THAT(img_pixel_size(simg), Eq(img_pixel_size(img)));
//     pxlsz = img_pixel_size(simg);

//     struct img_it *it = img_iterator(img);
//     ASSERT_THAT(it, Not(IsNull()));

//     struct img_it *sit = img_iterator(simg);
//     ASSERT_THAT(sit, Not(IsNull()));

//     while (img_it_has_next(it)) {
//         ASSERT_THAT(img_it_has_next(sit), Not(Eq(0)));
//         uint8_t *pxl = img_it_deref(it);
//         uint8_t *spxl = img_it_deref(sit);

//         for (int i = 0; i < pxlsz; ++i)
//             ASSERT_THAT(spxl[i], Eq(pxl[i]));

//         img_it_next(it);
//         img_it_next(sit);
//     }

//     img_destroy(img);
//     img_destroy(simg);
// }

// INSTANTIATE_TEST_SUITE_P(PPMTest, PPMTest,
//                          Values(PPMTestImg{ "blocks.binary.ppm" },
//                                 PPMTestImg{ "feep.ascii.ppm" },
//                                 PPMTestImg{ "fruit.binary.ppm" },
//                                 PPMTestImg{ "haus.ascii.ppm" },
//                                 PPMTestImg{ "haus.binary.ppm" },
//                                 PPMTestImg{ "ppmex1.ascii.ppm" },
//                                 PPMTestImg{ "ppmex255.ascii.ppm" },
//                                 PPMTestImg{ "ppmex255.binary.ppm" },
//                                 PPMTestImg{ "prague.binary.ppm" },
//                                 PPMTestImg{ "squares.binary.ppm" }),
//                          [](const testing::TestParamInfo<PPMTestImg> &info) {
//                              std::string fname = info.param.file();

//                              fname = fname.substr(0, fname.find_last_of('.'));
//                              std::replace(fname.begin(), fname.end(), '.', '_');

//                              return fname;
//                          });
