#pragma once

#include "Config.h"

namespace gomoku {

// 一局对局的完整可变状态：棋盘、模式、AI 设置、当前玩家与计时。
struct GameState {
    Board board{};                 // 棋盘，初始全空
    int gameMode = kModePve;       // 默认人机对战
    int aiDifficulty = kAiNormal;  // 默认普通难度
    bool aiPlaysBlack = true;      // true：AI 执黑（先手）；false：AI 执白（后手）
    bool timeLimitEnabled = true;  // 是否启用单步限时
    int currentPlayer = kBlack;    // 当前该落子的玩家（kBlack / kWhite）
    int stepTimeLimit = kDefaultStepSeconds; // 每步限时秒数
    int timeLeft = 0;              // 当前回合剩余秒数
    unsigned long startTick = 0;   // 当前回合开始的系统时刻（GetTickCount）

    // 清空棋盘，并把回合重置为黑棋先手
    void resetBoard();

    // 切换当前玩家
    void switchPlayer();

    // 返回另一名玩家的颜色
    int otherPlayer(int player) const;

    bool isCellEmpty(int row, int col) const;

    // 指定颜色的棋子当前是否由 AI 控制
    bool isAiControlled(int player) const;

    // 人机模式下，当前回合是否轮到 AI
    bool isAiMove() const;

    // 从当前时刻重新开始计算本回合时间
    void startPlayerClock();
};

} // namespace gomoku
