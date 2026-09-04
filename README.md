# Gobang-CPP

一个基于 C++ 和 EasyX 图形库开发的 Windows 桌面五子棋项目。

## 项目功能

- 支持玩家对玩家（PvP）模式；
- 支持玩家对 AI（PvE）模式；
- 支持 AI 演示模式；
- 支持简单、普通、困难三种 AI 难度；
- 支持 AI 执黑或执白；
- 支持黑白棋落子、横向、纵向和斜向五连判胜；
- 支持每步 60 秒倒计时，可通过 `T` 键开关；
- 支持 `R` 键重新开始、`ESC` 键退出。

## 运行环境

- Windows 10 / Windows 11
- Visual Studio（安装“使用 C++ 的桌面开发”）
- EasyX 图形库

## 运行方式

1. 安装 EasyX；
2. 使用 Visual Studio 打开项目；
3. 确保 `ChessUnderground.bmp` 位于程序运行目录；
4. 编译并运行项目。

## 游戏截图

### 主菜单

![游戏主菜单](assets/screenshots/menu.png)

### 对局进行中

![对局进行中](assets/screenshots/gameplay.png)

### 胜负判定

![胜负判定](assets/screenshots/win.png)
