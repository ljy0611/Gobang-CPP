#include "MainMenu.h"

#include <graphics.h>
#include <Windows.h>

#include "GameState.h"
#include "GameUi.h"

namespace gomoku {
namespace menu {

namespace {

// 菜单文字布局与点击区域（沿用原版界面）
constexpr int kTextX = 530;
constexpr int kTitleX = 515;

void drawTimeLimitOption(bool enabled) {
    setfillcolor(BLACK);
    solidrectangle(530, 460, 700, 490);
    settextcolor(WHITE);
    outtextxy(530, 460, enabled ? L"T. 已启用" : L"T. 已禁用");
}

void drawMainMenu(bool timeLimitEnabled) {
    ui::loadBackgroundToWindow();

    // 右侧黑色面板
    setfillcolor(BLACK);
    solidrectangle(500, 0, 700, 500);

    settextcolor(WHITE);
    outtextxy(515, 30, L"五子棋游戏");

    // 游戏模式
    outtextxy(515, 70, L"游戏模式:");
    outtextxy(530, 95, L"1. 玩家 vs 玩家");
    outtextxy(530, 120, L"2. 玩家 vs AI");
    outtextxy(530, 145, L"3. AI 演示模式");

    // AI 难度
    outtextxy(515, 175, L"AI难度:");
    outtextxy(530, 200, L"1. 简单");
    outtextxy(530, 225, L"2. 普通");
    outtextxy(530, 250, L"3. 困难");

    // AI 执棋颜色
    outtextxy(515, 280, L"AI执棋:");
    outtextxy(530, 305, L"1. AI执黑(先手)");
    outtextxy(530, 330, L"2. AI执白(后手)");

    outtextxy(515, 370, L"按S开始游戏");
    outtextxy(515, 395, L"按ESC退出");

    // 时间限制
    outtextxy(515, 435, L"时间限制:");
    drawTimeLimitOption(timeLimitEnabled);
}

} // namespace

bool runMainMenu(GameState& state) {
    initgraph(kWindowWidth, kMenuWindowHeight);
    drawMainMenu(state.timeLimitEnabled);

    bool start = false;
    bool quit = false;

    while (!start && !quit) {
        ExMessage msg;
        if (!peekmessage(&msg, EX_MOUSE | EX_KEY)) {
            Sleep(10); // 避免空转占用 CPU
            continue;
        }

        if (msg.message == WM_LBUTTONDOWN) {
            const int x = msg.x;
            const int y = msg.y;

            // 游戏模式：点击后立即开始（沿用原版交互）
            if (x >= kTextX && x <= 650) {
                if (y >= 95 && y <= 110) {
                    state.gameMode = kModePvp;
                    start = true;
                } else if (y >= 120 && y <= 135) {
                    state.gameMode = kModePve;
                    start = true;
                } else if (y >= 145 && y <= 160) {
                    state.gameMode = kModeAiOnly;
                    start = true;
                }
            }

            // AI 难度
            if (x >= kTextX && x <= 650) {
                if (y >= 200 && y <= 215) {
                    state.aiDifficulty = kAiEasy;
                } else if (y >= 225 && y <= 240) {
                    state.aiDifficulty = kAiNormal;
                } else if (y >= 250 && y <= 265) {
                    state.aiDifficulty = kAiHard;
                }
            }

            // AI 执棋颜色
            if (x >= kTextX && x <= 650) {
                if (y >= 305 && y <= 320) {
                    state.aiPlaysBlack = true; // AI 执黑
                } else if (y >= 330 && y <= 345) {
                    state.aiPlaysBlack = false; // AI 执白
                }
            }

            // 时间限制开关
            if (x >= kTextX && x <= 650 && y >= 435 && y <= 450) {
                state.timeLimitEnabled = !state.timeLimitEnabled;
                drawTimeLimitOption(state.timeLimitEnabled);
            }

            // 点击“开始游戏”文字区域
            if (x >= kTitleX && x <= 600 && y >= 370 && y <= 385) {
                start = true;
            }
        } else if (msg.message == WM_KEYDOWN) {
            if (msg.vkcode == 'S') {
                start = true;
            } else if (msg.vkcode == VK_ESCAPE) {
                quit = true;
            } else if (msg.vkcode == 'T') {
                state.timeLimitEnabled = !state.timeLimitEnabled;
                drawTimeLimitOption(state.timeLimitEnabled);
            }
        }
    }

    closegraph();
    return start;
}

} // namespace menu
} // namespace gomoku
