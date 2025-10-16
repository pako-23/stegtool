extern "C" {
#include <img/img.h>
#include <img/png.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

class PNGTestImg {
public:
    PNGTestImg(const char *file, size_t width = 32, size_t height = 32)
            : file_{ file }
            , width_{ width }
            , height_{ height }
    {
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

class PNGTest : public TestWithParam<PNGTestImg> {};

TEST(PNGTest, ImgFromFileIvalidMagic)
{
    struct img_s *img = img_from_file("invalid-magic.png");

    ASSERT_THAT(img, IsNull());
}

TEST(PNGTest, NewPNGImgInvalidMagic)
{
    FILE *fp;
    struct img_s *img;

    fp = fopen("invalid-magic.png", "rb");
    ASSERT_THAT(fp, Not(IsNull()));

    img = (struct img_s *)png_img_new(fp);
    ASSERT_THAT(img, IsNull());
    (void)fclose(fp);
}

TEST_P(PNGTest, ImgFromFile)
{
    PNGTestImg param = GetParam();
    struct img_s *img = img_from_file(param.file());

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(param.width()));
    ASSERT_THAT(img_height(img), Eq(param.height()));

    img_destroy(img);
}

TEST_P(PNGTest, NewPNGImg)
{
    FILE *fp;
    struct img_s *img;
    PNGTestImg param = GetParam();

    fp = fopen(param.file(), "rb");
    ASSERT_THAT(fp, Not(IsNull()));

    img = (struct img_s *)png_img_new(fp);
    (void)fclose(fp);

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(img_width(img), Eq(param.width()));
    ASSERT_THAT(img_height(img), Eq(param.height()));

    img_destroy(img);
}

TEST_P(PNGTest, Save)
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

TEST_P(PNGTest, PixelIteration)
{
    PNGTestImg param = GetParam();
    struct img_s *img = img_from_file(param.file());
    ASSERT_THAT(img, Not(IsNull()));
    size_t expected_pixels = param.width() * param.height();

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

INSTANTIATE_TEST_SUITE_P(
        PNGTest, PNGTest,
        Values(PNGTestImg{ "basn0g01.png" }, PNGTestImg{ "basn0g02.png" },
               PNGTestImg{ "basn0g04.png" }, PNGTestImg{ "basn0g08.png" },
               PNGTestImg{ "basn0g16.png" }, PNGTestImg{ "basn2c08.png" },
               PNGTestImg{ "basn2c16.png" }, PNGTestImg{ "basn3p01.png" },
               PNGTestImg{ "basn3p02.png" }, PNGTestImg{ "basn3p04.png" },
               PNGTestImg{ "basn3p08.png" }, PNGTestImg{ "basn4a08.png" },
               PNGTestImg{ "basn4a16.png" }, PNGTestImg{ "basn6a08.png" },
               PNGTestImg{ "basn6a16.png" }, PNGTestImg{ "ftbbn0g01.png" },
               PNGTestImg{ "ftbbn0g02.png" }, PNGTestImg{ "ftbbn0g04.png" },
               PNGTestImg{ "ftbbn2c16.png" }, PNGTestImg{ "ftbbn3p08.png" },
               PNGTestImg{ "ftbgn2c16.png" }, PNGTestImg{ "ftbgn3p08.png" },
               PNGTestImg{ "ftbrn2c08.png" }, PNGTestImg{ "ftbwn0g16.png" },
               PNGTestImg{ "ftbwn3p08.png" }, PNGTestImg{ "ftbyn3p08.png" },
               PNGTestImg{ "ftp0n0g08.png" }, PNGTestImg{ "ftp0n2c08.png" },
               PNGTestImg{ "ftp0n3p08.png" }, PNGTestImg{ "ftp1n3p08.png" },
               PNGTestImg{ "ibasn0g08.png" }, PNGTestImg{ "ibasn0g16.png" },
               PNGTestImg{ "ibasn2c08.png" }, PNGTestImg{ "ibasn2c16.png" },
               PNGTestImg{ "ibasn3p08.png" }, PNGTestImg{ "ibasn4a08.png" },
               PNGTestImg{ "ibasn4a16.png" }, PNGTestImg{ "ibasn6a08.png" },
               PNGTestImg{ "ibasn6a16.png" }, PNGTestImg{ "iftbbn2c16.png" },
               PNGTestImg{ "iftbbn3p08.png" }, PNGTestImg{ "iftbgn2c16.png" },
               PNGTestImg{ "iftbgn3p08.png" }, PNGTestImg{ "iftbrn2c08.png" },
               PNGTestImg{ "iftbwn0g16.png" }, PNGTestImg{ "iftbwn3p08.png" },
               PNGTestImg{ "iftbyn3p08.png" }, PNGTestImg{ "iftp0n0g08.png" },
               PNGTestImg{ "iftp0n2c08.png" }, PNGTestImg{ "iftp0n3p08.png" },
               PNGTestImg{ "iftp1n3p08.png" },
               PNGTestImg{ "cat.png", 320, 395 }),
        [](const testing::TestParamInfo<PNGTestImg> &info) {
            std::string fname = info.param.file();

            return fname.substr(0, fname.find_last_of('.'));
        });
