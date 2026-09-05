#pragma once

#include "Config.h"

namespace gomoku {

struct GameState;

namespace ui {

// 兼容两种存放位置的背景图加载：优先运行目录/工程根目录，其次 assets/。
void loadBackgroundToWindow();

// 只负责 EasyX 游戏画面的绘制，不包含规则和对局流程逻辑。
class GameUi {
public:
    void openGameWindow() const;
    void closeWindow() const;

    // 绘制完整对局画面：背景、棋盘、右侧信息与回合提示
    void drawScene(const GameState& state) const;

    // 在 (row, col) 播放落子动画并绘制棋子
    void drawPieceAnimation(int row, int col, int color) const;

    // 刷新右侧“当前回合”提示
    void drawTurnLabel(const GameState& state) const;

    // 刷新左下角倒计时
    void drawCountdown(const GameState& state) const;

    // 显示获胜文字；isAi 区分“AI 获胜”与“玩家获胜”
    void drawWinMessage(int winner, bool isAi) const;

private:
    void drawBoardGrid() const;
    void drawModeHint(const GameState& state) const;
};

} // namespace ui
} // namespace gomoku
