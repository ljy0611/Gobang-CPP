#include "GameState.h"

#include <Windows.h>

namespace gomoku {

void GameState::resetBoard() {
    for (auto& row : board) {
        row.fill(kEmpty);
    }
    currentPlayer = kBlack;
}

void GameState::switchPlayer() {
    currentPlayer = otherPlayer(currentPlayer);
}

int GameState::otherPlayer(int player) const {
    return (player == kBlack) ? kWhite : kBlack;
}

bool GameState::isCellEmpty(int row, int col) const {
    return row >= 0 && row < kBoardRows &&
           col >= 0 && col < kBoardCols &&
           board[row][col] == kEmpty;
}

bool GameState::isAiControlled(int player) const {
    if (gameMode == kModeAiOnly) {
        return true;
    }
    if (gameMode != kModePve) {
        return false;
    }
    return aiPlaysBlack ? (player == kBlack) : (player == kWhite);
}

bool GameState::isAiMove() const {
    return gameMode == kModePve && isAiControlled(currentPlayer);
}

void GameState::startPlayerClock() {
    startTick = ::GetTickCount();
}

} // namespace gomoku
