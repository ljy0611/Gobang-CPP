#pragma once

#include "Config.h"

namespace gomoku {

struct GameState;

namespace menu {

// 打开主菜单窗口并阻塞等待玩家选择。
// 返回 true 表示开始游戏；false 表示退出。
// 无论结果如何，返回前都会关闭菜单窗口。
bool runMainMenu(GameState& state);

} // namespace menu
} // namespace gomoku
