#pragma once

#include "Config.h"

namespace gomoku {

// 在 (row, col) 落子后，判断该子是否形成五连。
// 返回 kBlack / kWhite 表示胜方；kEmpty 表示尚未结束；-1 表示坐标越界。
int checkWin(const Board& board, int row, int col);

} // namespace gomoku
