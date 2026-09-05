#pragma once

#include "Config.h"

namespace gomoku {

struct GameState;

namespace ui {
class GameUi;
}

namespace session {

// 对局窗口已打开并绘制完成后，运行完整对局循环直到结束或退出。
void playGame(GameState& state, ui::GameUi& ui);

} // namespace session
} // namespace gomoku
