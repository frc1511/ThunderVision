#pragma once

#include <units/length.h>

struct Chessboard {
  std::uint32_t rows;
  std::uint32_t cols;
  units::meter_t sq_size;
};