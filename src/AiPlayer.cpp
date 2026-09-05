#include "AiPlayer.h"

#include <cstdlib>
#include <vector>

namespace gomoku {

namespace {

int opponentOf(int color) {
    return kBlack + kWhite - color;
}

} // namespace

int evaluatePosition(const Board& board, int row, int col, int color) {
    if (board[row][col] != kEmpty) {
        return -1; // 已有棋子，不参与评估
    }

    int score = 0;

    // 靠近棋盘中心的空位加分
    constexpr int centerRadius = 4;
    if (row >= kBoardRows / 2 - centerRadius &&
        row <= kBoardRows / 2 + centerRadius &&
        col >= kBoardCols / 2 - centerRadius &&
        col <= kBoardCols / 2 + centerRadius) {
        score += 5;
    }

    // 周围 5x5 范围内每颗同色棋子加分
    constexpr int neighborRadius = 2;
    for (int dr = -neighborRadius; dr <= neighborRadius; ++dr) {
        for (int dc = -neighborRadius; dc <= neighborRadius; ++dc) {
            const int nearRow = row + dr;
            const int nearCol = col + dc;
            if (nearRow >= 0 && nearRow < kBoardRows &&
                nearCol >= 0 && nearCol < kBoardCols &&
                board[nearRow][nearCol] == color) {
                score += 3;
            }
        }
    }

    return score;
}

AiMove chooseAiMove(const Board& board, int color, int difficulty) {
    // 简单模式：随机选一个空位
    if (difficulty == kAiEasy) {
        std::vector<AiMove> empties;
        for (int row = 0; row < kBoardRows; ++row) {
            for (int col = 0; col < kBoardCols; ++col) {
                if (board[row][col] == kEmpty) {
                    empties.push_back(AiMove{row, col});
                }
            }
        }
        if (!empties.empty()) {
            return empties[rand() % empties.size()];
        }
        return AiMove{}; // 棋盘已满时使用默认中心点兜底
    }

    // 普通 / 困难模式：遍历所有空位打分，取最高分
    AiMove best;
    int bestScore = -1;

    for (int row = 0; row < kBoardRows; ++row) {
        for (int col = 0; col < kBoardCols; ++col) {
            if (board[row][col] != kEmpty) {
                continue;
            }

            int score = evaluatePosition(board, row, col, color);

            // 困难模式额外考虑阻挡对手
            if (difficulty == kAiHard) {
                score = score * 2 + evaluatePosition(board, row, col, opponentOf(color));
            }

            if (score > bestScore) {
                bestScore = score;
                best = AiMove{row, col};
            }
        }
    }

    return best;
}

} // namespace gomoku
