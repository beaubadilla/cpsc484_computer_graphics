
///////////////////////////////////////////////////////////////////////////////
// gfxrasterize.hpp
//
// Line segment rasterization.
//
// This file builds upon gfximage.hpp, so you may want to familiarize
// yourself with that header before diving into this one.
//
// Students: all of your work should go in this file, and the only files that
// you need to modify in project 1 are this file, and README.md.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>

#include "gfximage.hpp"
#include "gfxpng.hpp"

namespace gfx {

void plotLineHigh(int x0, int y0, int x1, int y1,
                  hdr_rgb color, hdr_image& target) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int slopeDirection = 1;
  if (dx < 0) {
    slopeDirection = -1;
    dx *= -1;
  }
  int decider = 2*dx - dy;
  int x = x0;

  for (int y = y0; y <= y1; y++) {
    target.pixel(x, y, color);
    if (decider > 0) {
      x += slopeDirection;
      decider -= 2*dy;
    }
    decider += 2*dx;
  }
}
void plotLineLow(int x0, int y0, int x1, int y1,
                hdr_rgb color, hdr_image& target) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int slopeDirection = 1;
  if (dy < 0) {
    slopeDirection = -1;
    dy *= -1;
  }
  int decider = 2*dy - dx;
  int y = y0;

  for (int x = x0; x <= x1; x++) {
    target.pixel(x, y, color);
    if (decider > 0) {
      y += slopeDirection;
      decider -= 2*dx;
    }
    decider += 2*dy;
  }
}

// Draw a line segment from (x0, y0) to (x1, y1) inside image target, all
// with color.
//
// target must be non-empty.
// (x0, y0) and (x1, y1) must be valid coordinates in target.
// There is no restriction on how (x0, y0) and (x1, y1) must be oriented
// relative to each other.
//
void rasterize_line_segment(hdr_image& target,
                            unsigned x0, unsigned y0,
                            unsigned x1, unsigned y1,
                            const hdr_rgb& color) {

  assert(!target.is_empty());
  assert(target.is_xy(x0, y0));
  assert(target.is_xy(x1, y1));

  // Primarily referenced:
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  int diffX = x1 - x0;          // needed for std::abs
  int diffY = y1 - y0;          // needed for std::abs
  int deltax = abs(diffX);
  int deltay = abs(diffY);

  if (deltax == 0) {            // vertical line
    for (int yaxis = fmin(y0,y1); yaxis <= fmax(y0,y1); yaxis++) {
      target.pixel(x0, yaxis, color);
    }
  }
  else if (deltay == 0) {       // horizontal line
    for (int xaxis = fmin(x0,x1); xaxis <= fmax(x0,x1); xaxis++) {
      target.pixel(xaxis, y0, color);
    }
  }
  else if (deltay < deltax) {   // slope < 1
    if (x0 > x1) plotLineLow(x1,y1,x0,y0,color,target);
    else plotLineLow(x0,y0,x1,y1,color,target);
  }
  else {                        // slope >= 1
    if (y0 > y1) plotLineHigh(x1,y1,x0,y0,color,target);
    else plotLineHigh(x0,y0,x1,y1,color,target);
  }
}

// Convenience function to create many images, each containing one rasterized
// line segment, and write them to PNG files, for the purposes of unit testing.
bool write_line_segment_cases(const std::string& filename_prefix) {
  for (unsigned end_x = 0; end_x <= 10; ++end_x) {
    for (unsigned end_y = 0; end_y <= 10; ++end_y) {
      hdr_image img(11, 11, gfx::SILVER);
      rasterize_line_segment(img, 5, 5, end_x, end_y, gfx::RED);
      std::string filename = (filename_prefix
                              + "-" + std::to_string(end_x)
                              + "-" + std::to_string(end_y)
                              + ".png");
      if (!write_png(img, filename)) {
        return false;
      }
    }
  }
  return true;
}

} // namespace gfx
