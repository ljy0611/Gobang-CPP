#include "Rules.h"

namespace gomoku {

namespace {

bool inBoard(int row, int col) {
    return row >= 0 && row < kBoardRows &&
           col >= 0 && col < kBoardCols;
}

} // namespace

int checkWin(const Board& board, int row, int col) {
    if (!inBoard(row, col)) {
        return -1;
    }

    const int player = board[row][col];
    if (player == kEmpty) {
        return kEmpty;
    }

    // 需要检查的四个方向：上、右、右上、左上；反方向对称检查即可覆盖全部直线
    constexpr int directions[4][2] = {
        {-1, 0},
        {0, 1},
        {-1, 1},
        {-1, -1},
    };

    for (const auto& dir : directions) {
        int count = 1;

        // 沿正方向延伸
        int rowNext = row + dir[0];
        int colNext = col + dir[1];
        while (inBoard(rowNext, colNext) && board[rowNext][colNext] == player) {
            ++count;
            rowNext += dir[0];
            colNext += dir[1];
        }

        // 沿反方向延伸
        rowNext = row - dir[0];
        colNext = col - dir[1];
        while (inBoard(rowNext, colNext) && board[rowNext][colNext] == player) {
            ++count;
            rowNext -= dir[0];
            colNext -= dir[1];
        }

        if (count >= kWinCount) {
            return player;
        }
    }

    return kEmpty;
}

} // namespace gomoku
