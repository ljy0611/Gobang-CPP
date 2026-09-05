#pragma once

#include "Config.h"

namespace gomoku {

struct AiMove {
    int row = kBoardRows / 2;
    int col = kBoardCols / 2;
};

// 评估空位 (row, col) 对 color 的价值，数值越大越优先
int evaluatePosition(const Board& board, int row, int col, int color);

// 按难度为 color 选择一个落点
AiMove chooseAiMove(const Board& board, int color, int difficulty);

} // namespace gomoku
