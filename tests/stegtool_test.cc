extern "C" {
#include <img/img.h>
#include <stegtool.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <cstdint>

using namespace testing;

class StegtoolTest : public TestWithParam<const char *> {};

TEST_P(StegtoolTest, EmbedValidMsg)
{
    const char *msg = "some message to embed";
    struct img_s *img = img_from_file(GetParam());

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg, strlen(msg)), Eq(0));

    img_destroy(img);
}

TEST_P(StegtoolTest, MsgTooLong)
{
    struct img_s *img = img_from_file(GetParam());
    uint32_t len = img_height(img) * img_width(img) * img_pixel_size(img);
    std::string msg(len, 0);

    std::generate_n(msg.begin(), len, []() -> char { return 'a'; });

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg.c_str(), len), Lt(0));

    img_destroy(img);
}

TEST_P(StegtoolTest, ExtractLength)
{
    const char *msg = "some message to embed";
    struct img_s *img = img_from_file(GetParam());
    uint32_t len;

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg, strlen(msg)), Eq(0));

    ASSERT_THAT(sg_extract_len(img, &len), Eq(0));
    ASSERT_THAT(len, Eq(strlen(msg)));

    img_destroy(img);
}

TEST_P(StegtoolTest, ExtractMsg)
{
    const char *msg = "some message to embed";
    struct img_s *img = img_from_file(GetParam());
    char *extracted;
    uint32_t len;

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg, strlen(msg)), Eq(0));

    ASSERT_THAT(sg_extract_len(img, &len), Eq(0));
    ASSERT_THAT(len, Eq(strlen(msg)));

    extracted = new char[len + 1];

    ASSERT_THAT(sg_extract_msg(img, extracted, len), Eq(0));
    extracted[len] = '\0';
    ASSERT_THAT(extracted, StrEq(msg));

    delete[] extracted;
    img_destroy(img);
}

INSTANTIATE_TEST_SUITE_P(
        Stegtool, StegtoolTest,
        Values("basn0g01.png", "basn0g02.png", "basn0g04.png", "basn0g08.png",
               "basn0g16.png", "basn2c08.png", "basn2c16.png", "basn3p01.png",
               "basn3p02.png", "basn3p04.png", "basn3p08.png", "basn4a08.png",
               "basn4a16.png", "basn6a08.png", "basn6a16.png", "cat.png",
               "ftbbn0g01.png", "ftbbn0g02.png", "ftbbn0g04.png",
               "ftbbn2c16.png", "ftbbn3p08.png", "ftbgn2c16.png",
               "ftbgn3p08.png", "ftbrn2c08.png", "ftbwn0g16.png",
               "ftbwn3p08.png", "ftbyn3p08.png", "ftp0n0g08.png",
               "ftp0n2c08.png", "ftp0n3p08.png", "ftp1n3p08.png",
               "ibasn0g08.png", "ibasn0g16.png", "ibasn2c08.png",
               "ibasn2c16.png", "ibasn3p08.png", "ibasn4a08.png",
               "ibasn4a16.png", "ibasn6a08.png", "ibasn6a16.png",
               "iftbbn2c16.png", "iftbbn3p08.png", "iftbgn2c16.png",
               "iftbgn3p08.png", "iftbrn2c08.png", "iftbwn0g16.png",
               "iftbwn3p08.png", "iftbyn3p08.png", "iftp0n0g08.png",
               "iftp0n2c08.png", "iftp0n3p08.png", "iftp1n3p08.png", "cat.jpg"),
        [](const testing::TestParamInfo<const char *> &info) {
            std::string fname = info.param;

            std::replace(fname.begin(), fname.end(), '.', '_');
            return fname;
        });
