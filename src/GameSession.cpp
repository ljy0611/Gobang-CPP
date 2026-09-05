#include "GameSession.h"

#include <graphics.h>
#include <Windows.h>
#include <ctime>

#include "AiPlayer.h"
#include "GameState.h"
#include "GameUi.h"
#include "Rules.h"

namespace gomoku {
namespace session {

namespace {

// AI 思考延迟（毫秒），让落子看起来更自然
constexpr int kAiThinkDelayMs = 300;

// 胜负提示出现后，等待玩家在游戏窗口内按键或点击再关闭
void waitForAnyInputToContinue() {
    ExMessage msg;
    bool gotInput = false;
    while (!gotInput) {
        while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
            if (msg.message == WM_KEYDOWN || msg.message == WM_LBUTTONDOWN) {
                gotInput = true;
                break;
            }
        }
        if (!gotInput) {
            Sleep(10);
        }
    }
}

bool convertMouseToBoard(int mouseX, int mouseY, int& outRow, int& outCol) {
    // 计算最近的棋盘交叉点
    const int closestX = (mouseX + kCellSize / 2) / kCellSize;
    const int closestY = (mouseY + kCellSize / 2) / kCellSize;

    const int actualX = closestX * kCellSize;
    const int actualY = closestY * kCellSize;

    // 允许 10 像素的点击误差
    constexpr int kClickTolerance = 10;
    const int dx = mouseX - actualX;
    const int dy = mouseY - actualY;
    if (dx <= kClickTolerance && dx >= -kClickTolerance &&
        dy <= kClickTolerance && dy >= -kClickTolerance) {
        outRow = closestY - 1;
        outCol = closestX - 1;
        return outRow >= 0 && outRow < kBoardRows &&
               outCol >= 0 && outCol < kBoardCols;
    }
    return false;
}

// 执行一步 AI：思考、落子、渲染并判断胜负。
// 返回 true 表示 AI 已经获胜，调用方应结束对局。
bool playAiTurn(GameState& state, ui::GameUi& ui, int color) {
    Sleep(kAiThinkDelayMs);

    AiMove move = chooseAiMove(state.board, color, state.aiDifficulty);

    // 兜底：理论上 AI 总会选中空位；若出现异常占用，则扫描第一个空位
    if (!state.isCellEmpty(move.row, move.col)) {
        bool found = false;
        for (int row = 0; row < kBoardRows && !found; ++row) {
            for (int col = 0; col < kBoardCols && !found; ++col) {
                if (state.isCellEmpty(row, col)) {
                    move.row = row;
                    move.col = col;
                    found = true;
                }
            }
        }
        if (!found) {
            return false; // 棋盘已满，无合法落点
        }
    }

    state.board[move.row][move.col] = color;
    ui.drawPieceAnimation(move.row, move.col, color);

    if (checkWin(state.board, move.row, move.col) == color) {
        ui.drawWinMessage(color, /*isAi=*/true);
        waitForAnyInputToContinue();
        return true;
    }
    return false;
}

} // namespace

void playGame(GameState& state, ui::GameUi& ui) {
    // 初始化随机种子，让简单模式每次落子位置不同
    srand(static_cast<unsigned>(time(nullptr)));

    // 开局：AI 执黑且为 PVE / AI 演示时，AI 先走
    if (state.aiPlaysBlack &&
        (state.gameMode == kModePve || state.gameMode == kModeAiOnly) &&
        state.currentPlayer == kBlack) {
        if (playAiTurn(state, ui, kBlack)) {
            return;
        }
        state.currentPlayer = kWhite;
        state.startPlayerClock();
    }

    bool running = true;

    while (running) {
        // 非 AI 演示模式下，给人类玩家做超时倒计时
        if (state.gameMode != kModeAiOnly && !state.isAiMove()) {
            const unsigned long now = GetTickCount();
            state.timeLeft =
                state.stepTimeLimit - static_cast<int>((now - state.startTick) / 1000);

            if (state.timeLimitEnabled && state.timeLeft <= 0) {
                MessageBoxW(nullptr, L"您已超时！已切换另一位玩家落子！", L"提示", MB_OK);

                state.switchPlayer();
                state.startPlayerClock();

                // 超时切到 AI 回合时，AI 直接落子
                if (state.gameMode == kModePve && state.isAiMove()) {
                    if (playAiTurn(state, ui, state.currentPlayer)) {
                        running = false;
                        break;
                    }
                    state.switchPlayer();
                    state.startPlayerClock();
                    continue;
                }

                ui.drawCountdown(state);
                continue;
            }

            if (state.timeLimitEnabled) {
                ui.drawCountdown(state);
            }
        }

        // AI 演示：双方都由 AI 自动落子
        if (state.gameMode == kModeAiOnly) {
            if (playAiTurn(state, ui, state.currentPlayer)) {
                running = false;
                break;
            }
            state.switchPlayer();
            continue;
        }

        // 人机模式轮到 AI
        if (state.isAiMove()) {
            if (playAiTurn(state, ui, state.currentPlayer)) {
                running = false;
                break;
            }
            state.switchPlayer();
            state.startPlayerClock();
            continue;
        }

        // 玩家回合：响应鼠标
        ExMessage msg;
        if (peekmessage(&msg, EX_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                int row = 0;
                int col = 0;
                if (convertMouseToBoard(msg.x, msg.y, row, col) &&
                    state.isCellEmpty(row, col)) {
                    const int player = state.currentPlayer;
                    state.board[row][col] = player;
                    ui.drawPieceAnimation(row, col, player);

                    if (checkWin(state.board, row, col) == player) {
                        ui.drawWinMessage(player, /*isAi=*/false);
                        waitForAnyInputToContinue();
                        running = false;
                        break;
                    }

                    state.switchPlayer();
                    state.startPlayerClock();
                    ui.drawTurnLabel(state);
                }
            }
        }

        // 玩家回合：响应键盘
        ExMessage keyMsg;
        if (peekmessage(&keyMsg, EX_KEY) && keyMsg.message == WM_KEYDOWN) {
            if (keyMsg.vkcode == 'R') {
                state.resetBoard();
                ui.drawScene(state);
                state.startPlayerClock();

                if (state.aiPlaysBlack &&
                    (state.gameMode == kModePve || state.gameMode == kModeAiOnly)) {
                    if (playAiTurn(state, ui, kBlack)) {
                        running = false;
                        break;
                    }
                    state.currentPlayer = kWhite;
                }
            } else if (keyMsg.vkcode == VK_ESCAPE) {
                running = false;
            }
        }

        // 玩家等待输入时短暂休眠，避免空转占用 CPU
        Sleep(10);
    }
}

} // namespace session
} // namespace gomoku
