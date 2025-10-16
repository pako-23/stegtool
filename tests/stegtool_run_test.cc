#include <cstdio>
#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

extern "C" {
int run(int argc, const char *argv[]);
}

using namespace testing;

class MainTest : public TestWithParam<const char *> {};

TEST_P(MainTest, EncodeDecode)
{
    std::string fname = GetParam();
    std::string out = "out-" + fname;
    std::string redirect = GetParam();
    const char *argv1[] = {
        "./stegtool",  "-m", "some message to hide", "-o", out.c_str(),
        fname.c_str(), NULL,
    };
    const char *argv2[] = {
        "./stegtool",
        "-d",
        out.c_str(),
        NULL,
    };
    int res;

    std::replace(redirect.begin(), redirect.end(), '.', '_');
    redirect.append(".out");

    res = run(6, argv1);
    EXPECT_THAT(res, Eq(EXIT_SUCCESS));

    testing::internal::CaptureStdout();
    res = run(3, argv2);
    EXPECT_THAT(res, Eq(EXIT_SUCCESS));
    std::string msg = testing::internal::GetCapturedStdout();
    msg.pop_back();

    EXPECT_THAT(msg, StrEq(argv1[2]));
}

INSTANTIATE_TEST_SUITE_P(
        Stegtool, MainTest,
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
               "iftp0n2c08.png", "iftp0n3p08.png", "iftp1n3p08.png"),
        [](const testing::TestParamInfo<const char *> &info) {
            std::string fname = info.param;

            std::replace(fname.begin(), fname.end(), '.', '_');
            return fname;
        });
