#include "GameState.h"
#include "GameSession.h"
#include "GameUi.h"
#include "MainMenu.h"

int main() {
    using namespace gomoku;

    GameState state;

    // 主菜单（内部负责打开/关闭菜单窗口）
    if (!menu::runMainMenu(state)) {
        return 0;
    }

    // 进入对局窗口
    ui::GameUi gameUi;
    gameUi.openGameWindow();
    gameUi.drawScene(state);
    state.startPlayerClock();

    session::playGame(state, gameUi);

    gameUi.closeWindow();
    return 0;
}
