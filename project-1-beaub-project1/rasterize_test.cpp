
#include <cassert>
#include <cstdio> // for remove()

#include "gtest/gtest.h"

#include "gfximage.hpp"
#include "gfxrasterize.hpp"

using namespace gfx;

// Test fixture to create all the got-...png files before checking the unit
// tests.
class GotImagesFixture : public ::testing::Test {
protected:
  void SetUp() override {
    bool ok = write_line_segment_cases("got");
    assert(ok);
  }
};

class RasterizeLineSinglePixel     : public GotImagesFixture { };
class RasterizeLineDifferentColors : public GotImagesFixture { };
class RasterizeLineHorizontal      : public GotImagesFixture { };
class RasterizeLineVertical        : public GotImagesFixture { };
class RasterizeLineDiagonal        : public GotImagesFixture { };
class RasterizeLineGeneralSlope    : public GotImagesFixture { };

TEST(GfxProvidedCodeTest, IntegersApproxEqual) {
  EXPECT_TRUE(approx_equal(0.0, 0.0, .01));
  EXPECT_TRUE(approx_equal(3.0, 3.0, .01));
  EXPECT_TRUE(approx_equal(-3.0, -3.0, .01));
  EXPECT_TRUE(approx_equal(1E9, 1E9, .01));
  EXPECT_TRUE(approx_equal(-1E9, -1E9, .01));
  EXPECT_FALSE(approx_equal(0.0, 3.0, .01));
  EXPECT_FALSE(approx_equal(-3.0, 3.0, .01));
  for (int a = -100; a <= 100; ++a) {
    for (int b = -100; b <= 100; ++b) {
      EXPECT_EQ(bool(a == b), approx_equal(double(a), double(b), .001));
    }
  }
}

TEST(GfxProvidedCodeTest, FractionsApproxEqual) {
  EXPECT_TRUE(approx_equal(0.5, 0.5, .01));
  EXPECT_TRUE(approx_equal(0.5, 0.50001, .01));
  EXPECT_FALSE(approx_equal(0.5, 0.4, .01));
}

TEST(GfxProvidedCodeTest, SubnormalsApproxEqual) {
  // if either argument is subnormal, should return false
  EXPECT_FALSE(approx_equal(DOUBLE_INFINITY, DOUBLE_INFINITY, .01));
  EXPECT_FALSE(approx_equal(1.0, DOUBLE_INFINITY, .01));
  EXPECT_FALSE(approx_equal(DOUBLE_INFINITY, 1.0, .01));
  EXPECT_FALSE(approx_equal(DOUBLE_NEGATIVE_INFINITY, DOUBLE_NEGATIVE_INFINITY, .01));
  EXPECT_FALSE(approx_equal(DOUBLE_NAN, DOUBLE_NAN, .01));
}

TEST(GfxProvidedCodeTest, ImageTopLevelFunctions) {

  EXPECT_TRUE(is_hdr_intensity_valid(0.0));
  EXPECT_TRUE(is_hdr_intensity_valid(0.5));
  EXPECT_TRUE(is_hdr_intensity_valid(1.0));
  EXPECT_FALSE(is_hdr_intensity_valid(-1.0));
  EXPECT_FALSE(is_hdr_intensity_valid(1.1));
  EXPECT_FALSE(is_hdr_intensity_valid(1.1));
  EXPECT_FALSE(is_hdr_intensity_valid(FLOAT_INFINITY));
  EXPECT_FALSE(is_hdr_intensity_valid(FLOAT_NEGATIVE_INFINITY));
  EXPECT_FALSE(is_hdr_intensity_valid(FLOAT_NAN));

  EXPECT_EQ(0.0, byte_to_hdr(0));
  EXPECT_EQ(1.0, byte_to_hdr(255));
  EXPECT_TRUE(approx_equal(0.5f, byte_to_hdr(128), 0.1f));

  EXPECT_EQ(0, hdr_to_byte(0.0));
  EXPECT_EQ(255, hdr_to_byte(1.0));
  EXPECT_TRUE(approx_equal(128.0f, float(hdr_to_byte(0.5)), 1.0f));

  EXPECT_TRUE(hdr_intensity_approx_equal(0.0, 0.0, .01));
  EXPECT_TRUE(hdr_intensity_approx_equal(1.0, 1.0, .01));
  EXPECT_TRUE(hdr_intensity_approx_equal(0.5, 0.4999, .01));
  EXPECT_FALSE(hdr_intensity_approx_equal(0.0, 1.0, .01));
}

TEST(GfxProvidedCodeTest, HdrRgb) {
  { // constructor
    hdr_rgb black; // default constructor
    EXPECT_EQ(0.0, black.r());
    EXPECT_EQ(0.0, black.g());
    EXPECT_EQ(0.0, black.b());
    hdr_rgb color(0.0, 0.5, 1.0); // r, g, b constructor
    EXPECT_EQ(0.0, color.r());
    EXPECT_EQ(0.5, color.g());
    EXPECT_EQ(1.0, color.b());
    hdr_rgb copy(color); // copy constructor
    EXPECT_EQ(copy, color);
  }

  { // accessors and mutators
    hdr_rgb rgb(BLACK);
    EXPECT_EQ(BLACK, rgb);

    EXPECT_EQ(0.0, rgb.r());
    rgb.r(0.5);
    EXPECT_EQ(0.5, rgb.r());

    EXPECT_EQ(0.0, rgb.g());
    rgb.g(0.5);
    EXPECT_EQ(0.5, rgb.g());

    EXPECT_EQ(0.0, rgb.b());
    rgb.b(0.5);
    EXPECT_EQ(0.5, rgb.b());
  }

  { // operator==
    EXPECT_TRUE(BLACK == BLACK);
    EXPECT_TRUE(WHITE == WHITE);
    EXPECT_TRUE(MAROON == MAROON);
    EXPECT_FALSE(BLACK == WHITE);
    EXPECT_FALSE(WHITE == BLACK);
    EXPECT_FALSE(MAROON == BLACK);
  }

  { // approx_equal
    EXPECT_TRUE(BLACK.approx_equal(BLACK, .01));
    EXPECT_TRUE(WHITE.approx_equal(WHITE, .01));
    EXPECT_TRUE(MAROON.approx_equal(MAROON, .01));
    EXPECT_FALSE(BLACK.approx_equal(WHITE, .01));
    EXPECT_FALSE(WHITE.approx_equal(BLACK, .01));
    hdr_rgb a(0.0, 0.0, 0.005), b(0.0, 0.0, 0.006);
    EXPECT_TRUE(a.approx_equal(b, .01));
    EXPECT_TRUE(b.approx_equal(a, .01));
    EXPECT_FALSE(a.approx_equal(b, .0001));
    EXPECT_FALSE(b.approx_equal(a, .0001));
  }

  { // assign
    hdr_rgb rgb(MAROON);
    EXPECT_EQ(MAROON, rgb);
    rgb.assign(0.0, 0.5, 1.0);
    EXPECT_EQ(0.0, rgb.r());
    EXPECT_EQ(0.5, rgb.g());
    EXPECT_EQ(1.0, rgb.b());
  }

  { // begin, end
    hdr_rgb rgb(0.0, 0.5, 1.0);
    hdr_rgb::iterator ci = rgb.begin();
    EXPECT_NE(ci, rgb.end());
    EXPECT_EQ(0.0, *ci);
    ++ci;
    EXPECT_NE(ci, rgb.end());
    EXPECT_EQ(0.5, *ci);
    ++ci;
    EXPECT_NE(ci, rgb.end());
    EXPECT_EQ(1.0, *ci);
    ++ci;
    EXPECT_EQ(ci, rgb.end());

    std::vector<hdr_intensity> intensities;
    for (auto& i : rgb) {
      intensities.push_back(i);
    }
    EXPECT_EQ(3, intensities.size());
    EXPECT_EQ(0.0, intensities[0]);
    EXPECT_EQ(0.5, intensities[1]);
    EXPECT_EQ(1.0, intensities[2]);
  }

  { // fill
    hdr_rgb rgb(WHITE);
    rgb.fill(0.5);
    EXPECT_EQ(0.5, rgb.r());
    EXPECT_EQ(0.5, rgb.g());
    EXPECT_EQ(0.5, rgb.b());
  }

  { // from_bytes
    EXPECT_EQ(BLACK, hdr_rgb::from_bytes(0, 0, 0));
    EXPECT_EQ(WHITE, hdr_rgb::from_bytes(255, 255, 255));
    EXPECT_TRUE(hdr_rgb(25.0/255, 128.0/255, 220.0/255).approx_equal(hdr_rgb::from_bytes(25, 128, 220), .1));
  }

  { // from_hex
    EXPECT_EQ(hdr_rgb(0, 0, 0), hdr_rgb::from_hex(0x000000));
    EXPECT_EQ(hdr_rgb(1.0, 1.0, 1.0), hdr_rgb::from_hex(0xFFFFFF));
    EXPECT_EQ(hdr_rgb(1.0, 0.0, 0.0), hdr_rgb::from_hex(0xFF0000));
    EXPECT_EQ(hdr_rgb(0.0, 1.0, 0.0), hdr_rgb::from_hex(0x00FF00));
    EXPECT_EQ(hdr_rgb(0.0, 0.0, 1.0), hdr_rgb::from_hex(0x0000FF));
    EXPECT_TRUE(hdr_rgb(0.25, 0.25, 0.25).approx_equal(hdr_rgb::from_hex(0x404040), .1));
  }

  { // swap
    hdr_rgb a(MAROON), b(OLIVE);
    EXPECT_EQ(MAROON, a);
    EXPECT_EQ(OLIVE, b);
    a.swap(b);
    EXPECT_EQ(OLIVE, a);
    EXPECT_EQ(MAROON, b);
    b.swap(a);
    EXPECT_EQ(MAROON, a);
    EXPECT_EQ(OLIVE, b);
  }
}

TEST(GfxProvidedCodeTest, ColorConstants) {
  // just reference them to confirm that the constants exist and compile
  // cleanly
  hdr_rgb rgb;
  rgb = WHITE;
  rgb = SILVER;
  rgb = GRAY;
  rgb = BLACK;
  rgb = RED;
  rgb = MAROON;
  rgb = YELLOW;
  rgb = OLIVE;
  rgb = LIME;
  rgb = GREEN;
  rgb = AQUA;
  rgb = TEAL;
  rgb = BLUE;
  rgb = NAVY;
  rgb = FUSCHIA;
  rgb = PURPLE;
  EXPECT_EQ(PURPLE, rgb);
}

TEST(GfxProvidedCodeTest, HdrImage) {
  { // default constructor
    hdr_image empty;
    EXPECT_TRUE(empty.is_empty());
    EXPECT_EQ(0, empty.width());
    EXPECT_EQ(0, empty.height());
  }

  { // width-height constructor
    hdr_image img(10, 15, BLUE);
    EXPECT_FALSE(img.is_empty());
    EXPECT_EQ(10, img.width());
    EXPECT_EQ(15, img.height());
    EXPECT_TRUE(img.is_every_pixel(BLUE));
  }

  { // same-size constructor
    hdr_image first(10, 15, RED);
    EXPECT_FALSE(first.is_empty());
    EXPECT_EQ(10, first.width());
    EXPECT_EQ(15, first.height());
    EXPECT_TRUE(first.is_every_pixel(RED));

    hdr_image second(first, GREEN);
    EXPECT_FALSE(second.is_empty());
    EXPECT_EQ(10, second.width());
    EXPECT_EQ(15, second.height());
    EXPECT_TRUE(second.is_every_pixel(GREEN));
  }

  { // copy constructor
    hdr_image first(10, 15, RED);
    EXPECT_FALSE(first.is_empty());
    EXPECT_EQ(10, first.width());
    EXPECT_EQ(15, first.height());
    EXPECT_TRUE(first.is_every_pixel(RED));

    hdr_image second(first);
    EXPECT_FALSE(second.is_empty());
    EXPECT_EQ(10, second.width());
    EXPECT_EQ(15, second.height());
    EXPECT_TRUE(second.is_every_pixel(RED));

    EXPECT_EQ(first, second);
    EXPECT_TRUE(first.approx_equal(second, .01));
  }

  { // approx_equal
    hdr_image red(5, 5, RED),
              white(red, WHITE),
              off_white(white, hdr_rgb(0.999, 1.0, 1.0));
    EXPECT_TRUE(red.approx_equal(red, .01));
    EXPECT_FALSE(red.approx_equal(white, .01));
    EXPECT_FALSE(red.approx_equal(off_white, .01));
  }

  { // clear
    hdr_image img(5, 5, RED);
    EXPECT_FALSE(img.is_empty());
    img.clear();
    EXPECT_TRUE(img.is_empty());
    img.resize(6, 6);
    EXPECT_FALSE(img.is_empty());
    img.clear();
    EXPECT_TRUE(img.is_empty());
  }

  { // fill
    hdr_image img(5, 5, RED);
    EXPECT_TRUE(img.is_every_pixel(RED));
    img.fill(WHITE);
    EXPECT_TRUE(img.is_every_pixel(WHITE));
  }

  { // height
    EXPECT_EQ(4, hdr_image(5, 4, RED).height());
    EXPECT_EQ(0, hdr_image().height());
  }

  { // is_x, is_y, is_xy
    hdr_image nonempty(4, 3, RED);

    EXPECT_TRUE(nonempty.is_x(0));
    EXPECT_TRUE(nonempty.is_x(1));
    EXPECT_TRUE(nonempty.is_x(2));
    EXPECT_TRUE(nonempty.is_x(3));

    EXPECT_FALSE(nonempty.is_x(4));
    EXPECT_TRUE(nonempty.is_y(0));
    EXPECT_TRUE(nonempty.is_y(1));
    EXPECT_TRUE(nonempty.is_y(2));
    EXPECT_FALSE(nonempty.is_y(3));

    EXPECT_TRUE(nonempty.is_xy(0, 0));
    EXPECT_TRUE(nonempty.is_xy(0, 1));
    EXPECT_TRUE(nonempty.is_xy(0, 2));
    EXPECT_FALSE(nonempty.is_xy(0, 3));
    EXPECT_TRUE(nonempty.is_xy(1, 0));
    EXPECT_TRUE(nonempty.is_xy(1, 1));
    EXPECT_TRUE(nonempty.is_xy(1, 2));
    EXPECT_FALSE(nonempty.is_xy(1, 3));
    EXPECT_TRUE(nonempty.is_xy(2, 0));
    EXPECT_TRUE(nonempty.is_xy(2, 1));
    EXPECT_TRUE(nonempty.is_xy(2, 2));
    EXPECT_FALSE(nonempty.is_xy(2, 3));
    EXPECT_TRUE(nonempty.is_xy(3, 0));
    EXPECT_TRUE(nonempty.is_xy(3, 1));
    EXPECT_TRUE(nonempty.is_xy(3, 2));
    EXPECT_FALSE(nonempty.is_xy(3, 3));
    EXPECT_FALSE(nonempty.is_xy(4, 0));
  }

  { // is_empty
    EXPECT_TRUE(hdr_image().is_empty());
    EXPECT_FALSE(hdr_image(5, 5, RED).is_empty());
    EXPECT_FALSE(hdr_image(1, 5, RED).is_empty());
    EXPECT_FALSE(hdr_image(5, 1, RED).is_empty());
  }

  { // is_every_pixel
    hdr_image img(2, 2, BLACK);
    EXPECT_TRUE(img.is_every_pixel(BLACK));
    EXPECT_FALSE(img.is_every_pixel(OLIVE));
    img.pixel(0, 0, OLIVE);
    EXPECT_FALSE(img.is_every_pixel(BLACK));
    EXPECT_FALSE(img.is_every_pixel(OLIVE));
    img.pixel(0, 1, OLIVE);
    EXPECT_FALSE(img.is_every_pixel(BLACK));
    EXPECT_FALSE(img.is_every_pixel(OLIVE));
    img.pixel(1, 0, OLIVE);
    EXPECT_FALSE(img.is_every_pixel(BLACK));
    EXPECT_FALSE(img.is_every_pixel(OLIVE));
    img.pixel(1, 1, OLIVE);
    EXPECT_FALSE(img.is_every_pixel(BLACK));
    EXPECT_TRUE(img.is_every_pixel(OLIVE));
    hdr_image large(100, 100, RED);
    EXPECT_TRUE(large.is_every_pixel(RED));
    EXPECT_FALSE(large.is_every_pixel(OLIVE));
  }

  { // is_same_size
    EXPECT_TRUE(hdr_image(5, 4, RED).is_same_size(hdr_image(5, 4, RED)));
    EXPECT_TRUE(hdr_image().is_same_size(hdr_image()));
    EXPECT_FALSE(hdr_image(5, 4, RED).is_same_size(hdr_image(4, 5, RED)));
    EXPECT_FALSE(hdr_image(5, 4, RED).is_same_size(hdr_image()));
  }

  { // pixel
    hdr_image img(2, 2, RED);
    EXPECT_EQ(RED, img.pixel(0, 0));
    EXPECT_EQ(RED, img.pixel(0, 1));
    EXPECT_EQ(RED, img.pixel(1, 0));
    EXPECT_EQ(RED, img.pixel(1, 1));
    img.pixel(0, 0, WHITE);
    img.pixel(0, 1, GREEN);
    img.pixel(1, 0, BLUE);
    img.pixel(1, 1, YELLOW);
    EXPECT_EQ(WHITE, img.pixel(0, 0));
    EXPECT_EQ(GREEN, img.pixel(0, 1));
    EXPECT_EQ(BLUE, img.pixel(1, 0));
    EXPECT_EQ(YELLOW, img.pixel(1, 1));
  }

  { // resize
    // empty to nonempty
    hdr_image empty;
    EXPECT_TRUE(empty.is_empty());
    empty.resize(4, 3, WHITE);
    EXPECT_FALSE(empty.is_empty());
    EXPECT_EQ(4, empty.width());
    EXPECT_EQ(3, empty.height());
    EXPECT_TRUE(empty.is_every_pixel(WHITE));

    // grow with different color
    hdr_image white(1, 1, WHITE);
    white.resize(2, 2, RED);
    EXPECT_EQ(2, white.width());
    EXPECT_EQ(2, white.height());
    EXPECT_FALSE(white.is_every_pixel(WHITE));
    EXPECT_EQ(WHITE, white.pixel(0, 0));
    EXPECT_EQ(RED, white.pixel(0, 1));
    EXPECT_EQ(RED, white.pixel(1, 0));
    EXPECT_EQ(RED, white.pixel(1, 1));

    // grow with default color black
    hdr_image red(1, 1, RED);
    red.resize(2, 2);
    EXPECT_EQ(2, red.width());
    EXPECT_EQ(2, red.height());
    EXPECT_FALSE(red.is_every_pixel(RED));
    EXPECT_EQ(RED, red.pixel(0, 0));
    EXPECT_EQ(BLACK, red.pixel(0, 1));
    EXPECT_EQ(BLACK, red.pixel(1, 0));
    EXPECT_EQ(BLACK, red.pixel(1, 1));

    // grow a lot
    hdr_image big(4, 3, BLACK);
    EXPECT_EQ(4, big.width());
    EXPECT_EQ(3, big.height());
    big.resize(640, 480);
    EXPECT_EQ(640, big.width());
    EXPECT_EQ(480, big.height());
    EXPECT_TRUE(big.is_every_pixel(BLACK));

    // shrink
    hdr_image shrink(4, 3, YELLOW);
    EXPECT_EQ(4, shrink.width());
    EXPECT_EQ(3, shrink.height());
    shrink.resize(3, 2, BLACK);
    EXPECT_EQ(3, shrink.width());
    EXPECT_EQ(2, shrink.height());
    EXPECT_TRUE(shrink.is_every_pixel(YELLOW));
  }

  { // swap
    hdr_image red(4, 3, RED),
              yellow(5, 5, YELLOW);
    EXPECT_EQ(4, red.width());
    EXPECT_EQ(3, red.height());
    EXPECT_TRUE(red.is_every_pixel(RED));
    EXPECT_EQ(5, yellow.width());
    EXPECT_EQ(5, yellow.height());
    EXPECT_TRUE(yellow.is_every_pixel(YELLOW));
    red.swap(yellow);
    EXPECT_EQ(4, yellow.width());
    EXPECT_EQ(3, yellow.height());
    EXPECT_TRUE(yellow.is_every_pixel(RED));
    EXPECT_EQ(5, red.width());
    EXPECT_EQ(5, red.height());
    EXPECT_TRUE(red.is_every_pixel(YELLOW));
    yellow.swap(red);
    EXPECT_EQ(4, red.width());
    EXPECT_EQ(3, red.height());
    EXPECT_TRUE(red.is_every_pixel(RED));
    EXPECT_EQ(5, yellow.width());
    EXPECT_EQ(5, yellow.height());
    EXPECT_TRUE(yellow.is_every_pixel(YELLOW));

    hdr_image empty, nonempty(2, 2, WHITE);
    EXPECT_TRUE(empty.is_empty());
    EXPECT_FALSE(nonempty.is_empty());
    empty.swap(nonempty);
    EXPECT_FALSE(empty.is_empty());
    EXPECT_TRUE(nonempty.is_empty());
    empty.swap(nonempty);
    EXPECT_TRUE(empty.is_empty());
    EXPECT_FALSE(nonempty.is_empty());
  }

  { // width
    EXPECT_EQ(5, hdr_image(5, 4, RED).width());
    EXPECT_EQ(0, hdr_image().width());
  }
}

TEST(GfxProvidedCodeTest, PngRead) {
  { // invalid path
    auto png = read_png("<nonexistent>.png");
    EXPECT_FALSE(png);
  }

  { // 2x2.png
    auto png = read_png("2x2.png");
    EXPECT_TRUE(png);
    EXPECT_FALSE(png->is_empty());
    EXPECT_EQ(2, png->width());
    EXPECT_EQ(2, png->height());
    EXPECT_EQ(RED, png->pixel(0, 0));
    EXPECT_EQ(WHITE, png->pixel(1, 0));
    EXPECT_EQ(WHITE, png->pixel(0, 1));
    EXPECT_EQ(RED, png->pixel(1, 1));
  }
}

TEST(GfxProvidedCodeTest, PngWrite) {
  static const std::string PATH("test.png");
  hdr_image to_write(5, 4, BLUE);
  EXPECT_TRUE(write_png(to_write, PATH));
  auto read = read_png(PATH);
  EXPECT_TRUE(read);
  EXPECT_EQ(5, read->width());
  EXPECT_EQ(4, read->height());
  EXPECT_TRUE(read->is_every_pixel(BLUE));
  remove(PATH.c_str());
}

TEST_F(RasterizeLineSinglePixel, RasterizeLineSinglePixel) {
  ASSERT_TRUE(png_equal("expected-5-5.png", "got-5-5.png"));
}

TEST_F(RasterizeLineDifferentColors, RasterizeLineDifferentColors) {
  hdr_image img(3, 3, WHITE);
  rasterize_line_segment(img, 1, 1, 1, 1, OLIVE);
  EXPECT_FALSE(img.is_every_pixel(WHITE));
  EXPECT_EQ(WHITE, img.pixel(0, 0));
  EXPECT_EQ(WHITE, img.pixel(1, 0));
  EXPECT_EQ(WHITE, img.pixel(2, 0));
  EXPECT_EQ(WHITE, img.pixel(0, 1));
  EXPECT_EQ(OLIVE, img.pixel(1, 1));
  EXPECT_EQ(WHITE, img.pixel(2, 1));
  EXPECT_EQ(WHITE, img.pixel(0, 2));
  EXPECT_EQ(WHITE, img.pixel(1, 2));
  EXPECT_EQ(WHITE, img.pixel(2, 2));

  rasterize_line_segment(img, 1, 1, 1, 1, TEAL);
  EXPECT_FALSE(img.is_every_pixel(WHITE));
  EXPECT_EQ(WHITE, img.pixel(0, 0));
  EXPECT_EQ(WHITE, img.pixel(1, 0));
  EXPECT_EQ(WHITE, img.pixel(2, 0));
  EXPECT_EQ(WHITE, img.pixel(0, 1));
  EXPECT_EQ(TEAL , img.pixel(1, 1));
  EXPECT_EQ(WHITE, img.pixel(2, 1));
  EXPECT_EQ(WHITE, img.pixel(0, 2));
  EXPECT_EQ(WHITE, img.pixel(1, 2));
  EXPECT_EQ(WHITE, img.pixel(2, 2));

  rasterize_line_segment(img, 1, 1, 1, 1, PURPLE);
  EXPECT_FALSE(img.is_every_pixel(WHITE));
  EXPECT_EQ(WHITE, img.pixel(0, 0));
  EXPECT_EQ(WHITE, img.pixel(1, 0));
  EXPECT_EQ(WHITE, img.pixel(2, 0));
  EXPECT_EQ(WHITE, img.pixel(0, 1));
  EXPECT_EQ(PURPLE, img.pixel(1, 1));
  EXPECT_EQ(WHITE, img.pixel(2, 1));
  EXPECT_EQ(WHITE, img.pixel(0, 2));
  EXPECT_EQ(WHITE, img.pixel(1, 2));
  EXPECT_EQ(WHITE, img.pixel(2, 2));
}

TEST_F(RasterizeLineHorizontal, RasterizeLineHorizontal) {
  ASSERT_TRUE(png_equal("expected-0-5.png", "got-0-5.png"));
  ASSERT_TRUE(png_equal("expected-1-5.png", "got-1-5.png"));
  ASSERT_TRUE(png_equal("expected-2-5.png", "got-2-5.png"));
  ASSERT_TRUE(png_equal("expected-3-5.png", "got-3-5.png"));
  ASSERT_TRUE(png_equal("expected-4-5.png", "got-4-5.png"));
  ASSERT_TRUE(png_equal("expected-6-5.png", "got-6-5.png"));
  ASSERT_TRUE(png_equal("expected-7-5.png", "got-7-5.png"));
  ASSERT_TRUE(png_equal("expected-8-5.png", "got-8-5.png"));
  ASSERT_TRUE(png_equal("expected-9-5.png", "got-9-5.png"));
  ASSERT_TRUE(png_equal("expected-10-5.png", "got-10-5.png"));
}

TEST_F(RasterizeLineVertical, RasterizeLineVertical) {
  ASSERT_TRUE(png_equal("expected-5-0.png", "got-5-0.png"));
  ASSERT_TRUE(png_equal("expected-5-1.png", "got-5-1.png"));
  ASSERT_TRUE(png_equal("expected-5-2.png", "got-5-2.png"));
  ASSERT_TRUE(png_equal("expected-5-3.png", "got-5-3.png"));
  ASSERT_TRUE(png_equal("expected-5-4.png", "got-5-4.png"));
  ASSERT_TRUE(png_equal("expected-5-6.png", "got-5-6.png"));
  ASSERT_TRUE(png_equal("expected-5-7.png", "got-5-7.png"));
  ASSERT_TRUE(png_equal("expected-5-8.png", "got-5-8.png"));
  ASSERT_TRUE(png_equal("expected-5-9.png", "got-5-9.png"));
  ASSERT_TRUE(png_equal("expected-5-10.png", "got-5-10.png"));
}

TEST_F(RasterizeLineDiagonal, RasterizeLineDiagonal) {
  // northwest
  ASSERT_TRUE(png_equal("expected-4-4.png", "got-4-4.png"));
  ASSERT_TRUE(png_equal("expected-3-3.png", "got-3-3.png"));
  ASSERT_TRUE(png_equal("expected-2-2.png", "got-2-2.png"));
  ASSERT_TRUE(png_equal("expected-1-1.png", "got-1-1.png"));
  ASSERT_TRUE(png_equal("expected-0-0.png", "got-0-0.png"));

  // northeast
  ASSERT_TRUE(png_equal("expected-6-4.png", "got-6-4.png"));
  ASSERT_TRUE(png_equal("expected-7-3.png", "got-7-3.png"));
  ASSERT_TRUE(png_equal("expected-8-2.png", "got-8-2.png"));
  ASSERT_TRUE(png_equal("expected-9-1.png", "got-9-1.png"));
  ASSERT_TRUE(png_equal("expected-10-0.png", "got-10-0.png"));

  // southwest
  ASSERT_TRUE(png_equal("expected-4-6.png", "got-4-6.png"));
  ASSERT_TRUE(png_equal("expected-3-7.png", "got-3-7.png"));
  ASSERT_TRUE(png_equal("expected-2-8.png", "got-2-8.png"));
  ASSERT_TRUE(png_equal("expected-1-9.png", "got-1-9.png"));
  ASSERT_TRUE(png_equal("expected-0-10.png", "got-0-10.png"));

  // southeast
  ASSERT_TRUE(png_equal("expected-6-6.png", "got-6-6.png"));
  ASSERT_TRUE(png_equal("expected-7-7.png", "got-7-7.png"));
  ASSERT_TRUE(png_equal("expected-8-8.png", "got-8-8.png"));
  ASSERT_TRUE(png_equal("expected-9-9.png", "got-9-9.png"));
  ASSERT_TRUE(png_equal("expected-10-10.png", "got-10-10.png"));
}

TEST_F(RasterizeLineGeneralSlope, EntireNorthWestQuadrant) {
  ASSERT_TRUE(png_equal("expected-0-0.png", "got-0-0.png"));
  ASSERT_TRUE(png_equal("expected-1-0.png", "got-1-0.png"));
  ASSERT_TRUE(png_equal("expected-2-0.png", "got-2-0.png"));
  ASSERT_TRUE(png_equal("expected-3-0.png", "got-3-0.png"));
  ASSERT_TRUE(png_equal("expected-4-0.png", "got-4-0.png"));

  ASSERT_TRUE(png_equal("expected-0-1.png", "got-0-1.png"));
  ASSERT_TRUE(png_equal("expected-1-1.png", "got-1-1.png"));
  ASSERT_TRUE(png_equal("expected-2-1.png", "got-2-1.png"));
  ASSERT_TRUE(png_equal("expected-3-1.png", "got-3-1.png"));
  ASSERT_TRUE(png_equal("expected-4-1.png", "got-4-1.png"));

  ASSERT_TRUE(png_equal("expected-0-2.png", "got-0-2.png"));
  ASSERT_TRUE(png_equal("expected-1-2.png", "got-1-2.png"));
  ASSERT_TRUE(png_equal("expected-2-2.png", "got-2-2.png"));
  ASSERT_TRUE(png_equal("expected-3-2.png", "got-3-2.png"));
  ASSERT_TRUE(png_equal("expected-4-2.png", "got-4-2.png"));

  ASSERT_TRUE(png_equal("expected-0-3.png", "got-0-3.png"));
  ASSERT_TRUE(png_equal("expected-1-3.png", "got-1-3.png"));
  ASSERT_TRUE(png_equal("expected-2-3.png", "got-2-3.png"));
  ASSERT_TRUE(png_equal("expected-3-3.png", "got-3-3.png"));
  ASSERT_TRUE(png_equal("expected-4-3.png", "got-4-3.png"));

  ASSERT_TRUE(png_equal("expected-0-4.png", "got-0-4.png"));
  ASSERT_TRUE(png_equal("expected-1-4.png", "got-1-4.png"));
  ASSERT_TRUE(png_equal("expected-2-4.png", "got-2-4.png"));
  ASSERT_TRUE(png_equal("expected-3-4.png", "got-3-4.png"));
  ASSERT_TRUE(png_equal("expected-4-4.png", "got-4-4.png"));
}

TEST_F(RasterizeLineGeneralSlope, EntireNorthEastQuadrant) {
  ASSERT_TRUE(png_equal("expected-6-0.png", "got-6-0.png"));
  ASSERT_TRUE(png_equal("expected-7-0.png", "got-7-0.png"));
  ASSERT_TRUE(png_equal("expected-8-0.png", "got-8-0.png"));
  ASSERT_TRUE(png_equal("expected-9-0.png", "got-9-0.png"));
  ASSERT_TRUE(png_equal("expected-10-0.png", "got-10-0.png"));

  ASSERT_TRUE(png_equal("expected-6-1.png", "got-6-1.png"));
  ASSERT_TRUE(png_equal("expected-7-1.png", "got-7-1.png"));
  ASSERT_TRUE(png_equal("expected-8-1.png", "got-8-1.png"));
  ASSERT_TRUE(png_equal("expected-9-1.png", "got-9-1.png"));
  ASSERT_TRUE(png_equal("expected-10-1.png", "got-10-1.png"));

  ASSERT_TRUE(png_equal("expected-6-2.png", "got-6-2.png"));
  ASSERT_TRUE(png_equal("expected-7-2.png", "got-7-2.png"));
  ASSERT_TRUE(png_equal("expected-8-2.png", "got-8-2.png"));
  ASSERT_TRUE(png_equal("expected-9-2.png", "got-9-2.png"));
  ASSERT_TRUE(png_equal("expected-10-2.png", "got-10-2.png"));

  ASSERT_TRUE(png_equal("expected-6-3.png", "got-6-3.png"));
  ASSERT_TRUE(png_equal("expected-7-3.png", "got-7-3.png"));
  ASSERT_TRUE(png_equal("expected-8-3.png", "got-8-3.png"));
  ASSERT_TRUE(png_equal("expected-9-3.png", "got-9-3.png"));
  ASSERT_TRUE(png_equal("expected-10-3.png", "got-10-3.png"));

  ASSERT_TRUE(png_equal("expected-6-4.png", "got-6-4.png"));
  ASSERT_TRUE(png_equal("expected-7-4.png", "got-7-4.png"));
  ASSERT_TRUE(png_equal("expected-8-4.png", "got-8-4.png"));
  ASSERT_TRUE(png_equal("expected-9-4.png", "got-9-4.png"));
  ASSERT_TRUE(png_equal("expected-10-4.png", "got-10-4.png"));
}

TEST_F(RasterizeLineGeneralSlope, EntireSouthWestQuadrant) {
  ASSERT_TRUE(png_equal("expected-0-6.png", "got-0-6.png"));
  ASSERT_TRUE(png_equal("expected-1-6.png", "got-1-6.png"));
  ASSERT_TRUE(png_equal("expected-2-6.png", "got-2-6.png"));
  ASSERT_TRUE(png_equal("expected-3-6.png", "got-3-6.png"));
  ASSERT_TRUE(png_equal("expected-4-6.png", "got-4-6.png"));

  ASSERT_TRUE(png_equal("expected-0-7.png", "got-0-7.png"));
  ASSERT_TRUE(png_equal("expected-1-7.png", "got-1-7.png"));
  ASSERT_TRUE(png_equal("expected-2-7.png", "got-2-7.png"));
  ASSERT_TRUE(png_equal("expected-3-7.png", "got-3-7.png"));
  ASSERT_TRUE(png_equal("expected-4-7.png", "got-4-7.png"));

  ASSERT_TRUE(png_equal("expected-0-8.png", "got-0-8.png"));
  ASSERT_TRUE(png_equal("expected-1-8.png", "got-1-8.png"));
  ASSERT_TRUE(png_equal("expected-2-8.png", "got-2-8.png"));
  ASSERT_TRUE(png_equal("expected-3-8.png", "got-3-8.png"));
  ASSERT_TRUE(png_equal("expected-4-8.png", "got-4-8.png"));

  ASSERT_TRUE(png_equal("expected-0-9.png", "got-0-9.png"));
  ASSERT_TRUE(png_equal("expected-1-9.png", "got-1-9.png"));
  ASSERT_TRUE(png_equal("expected-2-9.png", "got-2-9.png"));
  ASSERT_TRUE(png_equal("expected-3-9.png", "got-3-9.png"));
  ASSERT_TRUE(png_equal("expected-4-9.png", "got-4-9.png"));

  ASSERT_TRUE(png_equal("expected-0-10.png", "got-0-10.png"));
  ASSERT_TRUE(png_equal("expected-1-10.png", "got-1-10.png"));
  ASSERT_TRUE(png_equal("expected-2-10.png", "got-2-10.png"));
  ASSERT_TRUE(png_equal("expected-3-10.png", "got-3-10.png"));
  ASSERT_TRUE(png_equal("expected-4-10.png", "got-4-10.png"));
}

TEST_F(RasterizeLineGeneralSlope, EntireSouthEastQuadrant) {
  ASSERT_TRUE(png_equal("expected-6-6.png", "got-6-6.png"));
  ASSERT_TRUE(png_equal("expected-7-6.png", "got-7-6.png"));
  ASSERT_TRUE(png_equal("expected-8-6.png", "got-8-6.png"));
  ASSERT_TRUE(png_equal("expected-9-6.png", "got-9-6.png"));
  ASSERT_TRUE(png_equal("expected-10-6.png", "got-10-6.png"));

  ASSERT_TRUE(png_equal("expected-6-7.png", "got-6-7.png"));
  ASSERT_TRUE(png_equal("expected-7-7.png", "got-7-7.png"));
  ASSERT_TRUE(png_equal("expected-8-7.png", "got-8-7.png"));
  ASSERT_TRUE(png_equal("expected-9-7.png", "got-9-7.png"));
  ASSERT_TRUE(png_equal("expected-10-7.png", "got-10-7.png"));

  ASSERT_TRUE(png_equal("expected-6-8.png", "got-6-8.png"));
  ASSERT_TRUE(png_equal("expected-7-8.png", "got-7-8.png"));
  ASSERT_TRUE(png_equal("expected-8-8.png", "got-8-8.png"));
  ASSERT_TRUE(png_equal("expected-9-8.png", "got-9-8.png"));
  ASSERT_TRUE(png_equal("expected-10-8.png", "got-10-8.png"));

  ASSERT_TRUE(png_equal("expected-6-9.png", "got-6-9.png"));
  ASSERT_TRUE(png_equal("expected-7-9.png", "got-7-9.png"));
  ASSERT_TRUE(png_equal("expected-8-9.png", "got-8-9.png"));
  ASSERT_TRUE(png_equal("expected-9-9.png", "got-9-9.png"));
  ASSERT_TRUE(png_equal("expected-10-9.png", "got-10-9.png"));

  ASSERT_TRUE(png_equal("expected-6-10.png", "got-6-10.png"));
  ASSERT_TRUE(png_equal("expected-7-10.png", "got-7-10.png"));
  ASSERT_TRUE(png_equal("expected-8-10.png", "got-8-10.png"));
  ASSERT_TRUE(png_equal("expected-9-10.png", "got-9-10.png"));
  ASSERT_TRUE(png_equal("expected-10-10.png", "got-10-10.png"));
}
/*
TEST(single_pixel, single_pixel) {
  hdr_image img(3, 3, BLACK);

  // (0, 0)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 0, 0, 0, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(RED   , img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (1, 0)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 1, 0, 1, 0, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(RED   , img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (2, 0)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 2, 0, 2, 0, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(RED   , img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (0, 1)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 1, 0, 1, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(RED   , img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (1, 1)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 1, 1, 1, 1, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(RED   , img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (2, 1)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 2, 1, 2, 1, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(RED   , img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (0, 2)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 2, 0, 2, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(RED   , img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (1, 2)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 1, 2, 1, 2, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(RED   , img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // (2, 2)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 2, 2, 2, 2, RED);
  EXPECT_FALSE(img.is_every_pixel(SILVER));
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(RED   , img.pixel(2, 2));
}

TEST(horizontal_lines, horizontal_lines) {
  hdr_image img(3, 3, BLACK);

  // width 3 at y=0
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 0, 2, 0, RED);
  EXPECT_EQ(RED   , img.pixel(0, 0));
  EXPECT_EQ(RED   , img.pixel(1, 0));
  EXPECT_EQ(RED   , img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // width 3 at y=1
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 1, 2, 1, RED);
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(RED   , img.pixel(0, 1));
  EXPECT_EQ(RED   , img.pixel(1, 1));
  EXPECT_EQ(RED   , img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // width 3 at y=2
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 2, 2, 2, RED);
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(RED   , img.pixel(0, 2));
  EXPECT_EQ(RED   , img.pixel(1, 2));
  EXPECT_EQ(RED   , img.pixel(2, 2));

  // width 2 at (1, 1)
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 1, 1, 2, 1, RED);
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(RED   , img.pixel(1, 1));
  EXPECT_EQ(RED   , img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));
}

TEST(vertical_lines, vertical_lines) {
  hdr_image img(3, 3, BLACK);

  // height 3 at x=0
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 0, 0, 0, 2, RED);
  EXPECT_EQ(RED   , img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(RED   , img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(RED   , img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // height 3 at x=1
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 1, 0, 1, 2, RED);
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(RED   , img.pixel(1, 0));
  EXPECT_EQ(SILVER, img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(RED   , img.pixel(1, 1));
  EXPECT_EQ(SILVER, img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(RED   , img.pixel(1, 2));
  EXPECT_EQ(SILVER, img.pixel(2, 2));

  // height 3 at x=2
  img.fill(SILVER);
  ASSERT_TRUE(img.is_every_pixel(SILVER));
  rasterize_line_segment(img, 2, 0, 2, 2, RED);
  EXPECT_EQ(SILVER, img.pixel(0, 0));
  EXPECT_EQ(SILVER, img.pixel(1, 0));
  EXPECT_EQ(RED   , img.pixel(2, 0));
  EXPECT_EQ(SILVER, img.pixel(0, 1));
  EXPECT_EQ(SILVER, img.pixel(1, 1));
  EXPECT_EQ(RED   , img.pixel(2, 1));
  EXPECT_EQ(SILVER, img.pixel(0, 2));
  EXPECT_EQ(SILVER, img.pixel(1, 2));
  EXPECT_EQ(RED   , img.pixel(2, 2));
}

TEST(shallow_upward, shallow_upward) {

  hdr_image img(9, 9, SILVER);
  rasterize_line_segment(img, 0, 4, 1, 8, RED);
  img.write_png(img, "+4+1.png");

}
*/
