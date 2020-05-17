
#include <cassert>

#include "gfxrasterize.hpp"

int main() {
  bool ok = gfx::write_line_segment_cases("expected");
  assert(ok);
  return 0;
}
