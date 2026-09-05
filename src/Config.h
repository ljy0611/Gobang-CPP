#pragma once

#include <array>

// 全局配置：棋盘尺寸、常用常量与棋盘数据类型。
// 业务模块（状态/规则/AI）只依赖本文件，避免反向依赖界面模块。
namespace gomoku {

// 棋盘尺寸（行数 / 列数）
constexpr int kBoardRows = 20;
constexpr int kBoardCols = 20;

// 棋盘格子的像素边长，同时用于鼠标坐标换算
constexpr int kCellSize = 25;

// 棋盘格子状态：空位 / 黑棋 / 白棋
constexpr int kEmpty = 0;
constexpr int kBlack = 1;
constexpr int kWhite = 2;

// 游戏模式
constexpr int kModePvp = 0;    // 双人对战
constexpr int kModePve = 1;    // 人机对战
constexpr int kModeAiOnly = 2; // AI 演示

// AI 难度
constexpr int kAiEasy = 0;   // 随机落子
constexpr int kAiNormal = 1; // 基础评估
constexpr int kAiHard = 2;   // 简单攻防

// 连成五子获胜
constexpr int kWinCount = 5;

// 窗口尺寸（像素）
constexpr int kWindowWidth = 700;
constexpr int kMenuWindowHeight = 500;
constexpr int kGameWindowHeight = 600;

// 单步限时（秒）
constexpr int kDefaultStepSeconds = 60;

// 棋盘类型：board[row][col]，取值为 kEmpty / kBlack / kWhite
using Board = std::array<std::array<int, kBoardCols>, kBoardRows>;

} // namespace gomoku
