#pragma once
#include "params.h"

// x: [N][64][8][8]; output: [N][64][1][1]
void gap_forward(data_t out[N][FIL][OUT_H][OUT_W], data_t in[N][FIL][IN_H][IN_W]);