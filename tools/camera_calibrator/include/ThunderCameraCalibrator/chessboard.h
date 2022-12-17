#pragma once

#include <units/length.h>

struct Chessboard {
  std::int32_t rows;
  std::int32_t cols;
  units::meter_t sq_size;
};
