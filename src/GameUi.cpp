#include "GameUi.h"

#include <graphics.h>
#include <Windows.h>
#include <wchar.h>

#include "GameState.h"

namespace gomoku {
namespace ui {

namespace {

// 棋盘交叉点到窗口坐标的换算
int pointX(int col) {
    return (col + 1) * kCellSize;
}

int pointY(int row) {
    return (row + 1) * kCellSize;
}

} // namespace

void loadBackgroundToWindow() {
    // 兼容旧版习惯（图片放在工程/运行目录），也支持新版 assets/ 目录。
    // 本版本 EasyX 的 loadimage 返回 void，无法检测失败，因此按顺序加载，
    // 后一次成功会覆盖前一次：若 assets/ 存在则最终使用 assets/ 版本。
    loadimage(nullptr, L"ChessUnderground.bmp");
    loadimage(nullptr, L"assets/ChessUnderground.bmp");
}

void GameUi::openGameWindow() const {
    initgraph(kWindowWidth, kGameWindowHeight);
}

void GameUi::closeWindow() const {
    closegraph();
}

void GameUi::drawScene(const GameState& state) const {
    loadBackgroundToWindow();
    drawBoardGrid();

    // 星位（天元与四角星）
    setfillcolor(BLACK);
    solidcircle(pointX(10), pointY(10), 5);
    solidcircle(pointX(5), pointY(5), 5);
    solidcircle(pointX(5), pointY(15), 5);
    solidcircle(pointX(15), pointY(5), 5);
    solidcircle(pointX(15), pointY(15), 5);

    // 右侧说明文字
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    drawModeHint(state);
    outtextxy(530, 120, L"按R重新开始");
    outtextxy(530, 150, L"按ESC退出");

    drawTurnLabel(state);
}

void GameUi::drawBoardGrid() const {
    setlinecolor(BLACK);

    // 横线
    for (int row = 0; row < kBoardRows; ++row) {
        const int y = pointY(row);
        line(kCellSize, y, kBoardCols * kCellSize, y);
    }

    // 竖线
    for (int col = 0; col < kBoardCols; ++col) {
        const int x = pointX(col);
        line(x, kCellSize, x, kBoardRows * kCellSize);
    }

    // 边框
    setlinestyle(PS_SOLID, 2);
    line(kBoardCols * kCellSize + kCellSize, 0,
         kBoardCols * kCellSize + kCellSize, kGameWindowHeight);
    line(kBoardCols * kCellSize, kCellSize,
         kBoardCols * kCellSize, kBoardRows * kCellSize);
    line(kCellSize, 0, kCellSize, kBoardRows * kCellSize);
    line(0, kBoardRows * kCellSize + kCellSize,
         kBoardCols * kCellSize + kCellSize,
         kBoardRows * kCellSize + kCellSize);
    line(kCellSize, kCellSize, kBoardCols * kCellSize, kCellSize);
    line(kCellSize, kBoardRows * kCellSize,
         kBoardCols * kCellSize, kBoardRows * kCellSize);
}

void GameUi::drawModeHint(const GameState& state) const {
    if (state.gameMode == kModePvp) {
        outtextxy(530, 30, L"玩家1：黑棋");
        outtextxy(530, 60, L"玩家2：白棋");
        outtextxy(530, 90, L"双方计时");
    } else if (state.gameMode == kModePve) {
        if (state.aiPlaysBlack) {
            outtextxy(530, 30, L"AI：黑棋(先手)");
            outtextxy(530, 60, L"玩家：白棋");
        } else {
            outtextxy(530, 30, L"玩家：黑棋(先手)");
            outtextxy(530, 60, L"AI：白棋");
        }
        outtextxy(530, 90, L"玩家计时");
    } else { // kModeAiOnly
        outtextxy(530, 30, L"AI演示模式");
        outtextxy(530, 60, L"无计时");
    }
}

void GameUi::drawTurnLabel(const GameState& state) const {
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    setfillcolor(WHITE);
    solidrectangle(530, 180, 700, 210);

    wchar_t text[64];
    swprintf_s(text, 64, L"当前回合: %s",
               state.currentPlayer == kBlack ? L"黑棋" : L"白棋");
    outtextxy(530, 180, text);
}

void GameUi::drawCountdown(const GameState& state) const {
    if (!state.timeLimitEnabled) {
        return;
    }

    setfillcolor(WHITE);
    solidrectangle(15, 550, 250, 580);
    setfont(20, 0, L"宋体");
    settextcolor(RED);

    wchar_t text[64];
    swprintf_s(text, 64, L"剩余时间:%d秒", state.timeLeft);
    outtextxy(15, 550, text);
}

void GameUi::drawWinMessage(int winner, bool isAi) const {
    settextcolor(BLACK);
    if (isAi) {
        outtextxy(515, 300, winner == kBlack ? L"黑棋(AI)获胜!" : L"白棋(AI)获胜!");
    } else {
        outtextxy(530, 300, winner == kBlack ? L"黑棋获胜!" : L"白棋获胜!");
    }
}

void GameUi::drawPieceAnimation(int row, int col, int color) const {
    const int x = pointX(col);
    const int y = pointY(row);

    // 由大到小逐层绘制，形成落子动画
    for (int radius = 11; radius > 7; --radius) {
        setcolor(LIGHTGRAY);
        setfillcolor(LIGHTGRAY);
        solidcircle(x, y, radius + 1);

        setfillcolor(color == kBlack ? BLACK : WHITE);
        solidcircle(x, y, radius);
        Sleep(15);
    }

    // 最终棋子
    setfillcolor(color == kBlack ? BLACK : WHITE);
    solidcircle(x, y, 10);
    setcolor(RED);
}

} // namespace ui
} // namespace gomoku
