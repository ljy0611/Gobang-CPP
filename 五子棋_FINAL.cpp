#include <graphics.h> 
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

// 棋盘大小定义
#define M 20
#define N 20
#define CELL_SIZE 25    // 格子边长

// 游戏模式定义
#define MODE_PVP 0     // 玩家对玩家
#define MODE_PVE 1     // 玩家对AI
#define MODE_AI_ONLY 2 // AI演示

// AI难度定义
#define AI_EASY 0      // 简单模式：随机落子
#define AI_NORMAL 1    // 普通模式：基础评估
#define AI_HARD 2      // 困难模式：简单攻防

// 全局变量
int timeLimitEnabled = 1;   //时限开关，1开0关
int board[M][N] = { 0 };      // 棋盘状态数组
int gameMode = MODE_PVE;    // 默认游戏模式：玩家对AI
int aiDifficulty = AI_NORMAL; // 默认AI难度：普通
int aiTurn = 0;             // AI执棋颜色，0黑1白
int currentPlayer = 1;      // 1黑2白
int timeLeft;               // 剩余时间
int stepTimeLimit = 60;     // 每步限时60秒
unsigned long startTime;    // 计时开始时间

// 绘制棋子动画（从大到小）
void Put_animation(int x, int y, int color) {
    int X = (y + 1) * CELL_SIZE;
    int Y = (x + 1) * CELL_SIZE;

    // 从最大半径减到最小半径
    for (int i = 11; i > 7; i--) {

        // 清除之前的圆
        setcolor(LIGHTGRAY);
        setfillcolor(LIGHTGRAY);
        solidcircle(X, Y, i + 1);

        // 绘制当前半径的圆
        if (color == 1) setfillcolor(BLACK);
        else setfillcolor(WHITE);

        solidcircle(X, Y, i);

        // 短延时，形成动画视觉效果
        Sleep(15);
    }

    // 最终绘制正常大小的棋子
    if (color == 1) setfillcolor(BLACK);
    else setfillcolor(WHITE);

    solidcircle(X, Y, 10);

    setcolor(RED);
}

// 在棋盘上绘制倒计时
void DrawTime() {
    if (timeLimitEnabled) {
        char buf[50];
        sprintf_s(buf, "剩余时间:%d秒", timeLeft);
        settextstyle(20, 0, "宋体");
        settextcolor(RED);
        outtextxy(15, 550, buf);
    }
}

// 玩家切换时计时重启
void switchPlayer() {
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
    startTime = GetTickCount();
}

// 初始化游戏界面
void initGame() {
    initgraph(700, 600);  // 创建700x600的窗口
    loadimage(NULL, "ChessUnderground.jpg");  // 加载背景图片

    // 绘制20*20的棋盘线
    setlinecolor(BLACK);
    for (int i = 0; i < M; i++) {
        line(25, (i + 1) * 25, N * 25, (i + 1) * 25); // 横线
    }
    for (int j = 0; j < N; j++) {
        line((j + 1) * 25, 25, (j + 1) * 25, M * 25); // 竖线
    }

    // 绘制边框
    setlinestyle(PS_SOLID, 2);
    line(N * 25 + 25, 0, N * 25 + 25, 600);
    line(N * 25, 25, N * 25, M * 25);
    line(25, 0, 25, M * 25);
    line(0, M * 25 + 25, N * 25 + 25, M * 25 + 25);
    line(25, 25, N * 25, 25);
    line(25, M * 25, N * 25, M * 25);

    //棋盘打点
    setfillcolor(BLACK);
    solidcircle(25 * 11, 25 * 11, 5);
    solidcircle(25 * 6, 25 * 6, 5);
    solidcircle(25 * 6, 25 * 16, 5);
    solidcircle(25 * 16, 25 * 6, 5);
    solidcircle(25 * 16, 25 * 16, 5);

    // 设置文字显示
    setbkmode(0);  // 文字背景透明
    settextcolor(BLACK);

    // 根据游戏模式显示不同的界面
    if (gameMode == MODE_PVP) {
        outtextxy(530, 30, "玩家1：黑棋");
        outtextxy(530, 60, "玩家2：白棋");
        outtextxy(530, 90, "双方计时");
    }
    else if (gameMode == MODE_PVE) {
        if (aiTurn == 0) {
            outtextxy(530, 30, "AI：黑棋(先手)");
            outtextxy(530, 60, "玩家：白棋");
            outtextxy(530, 90, "玩家计时");
        }
        else {
            outtextxy(530, 30, "玩家：黑棋(先手)");
            outtextxy(530, 60, "AI：白棋");
            outtextxy(530, 90, "玩家计时");
        }
    }
    else { // MODE_AI_ONLY
        outtextxy(530, 30, "AI演示模式");
        outtextxy(530, 60, "无计时");
    }

    // 通用控制提示
    outtextxy(530, 120, "按R重新开始");
    outtextxy(530, 150, "按ESC退出");

    // 显示当前玩家
    char playerText[50];
    sprintf_s(playerText, "当前回合: %s", currentPlayer == 1 ? "黑棋" : "白棋");
    outtextxy(530, 180, playerText);

    // 初始化计时器
    startTime = GetTickCount();
}

// 五子棋胜负判定函数
int judge(int ChessMap[M][N], int MapX, int MapY) {

    //判断该位置是否在期盼范围内
    if (MapX < 0 || MapX >= M || MapY < 0 || MapY >= N)
        return -1;  // 超出棋盘范围

    int chess = ChessMap[MapX][MapY];  // 当前落子的颜色

    if (chess == 0)
        return 0;  // 该位置没有棋子,不需要进行判断


    int num;  // 计数变量，用于记录一个方向上的连铸数

    /*判断竖直方向*/
    num = 1;  //当前位置已有棋子，计数变量初始化为1，表示连珠数记录从1开始
    // 向上计数
    for (int i = MapX - 1; i >= 0; i--) {
        if (ChessMap[i][MapY] == chess)
            num++;//若有相同的棋子，则计数变量自增
        else break;//若没有相同的棋子，则结束计数
    }
    // 向下计数
    for (int i = MapX + 1; i < M; i++) {
        if (ChessMap[i][MapY] == chess)
            num++;
        else break;
    }
    //判断竖直方向总连珠数是否满足获胜条件（>=5）
    if (num >= 5)
        return chess;  // 返回获胜方


    /*2. 判断水平方向*/
    num = 1;//重置计数变量
    // 向左计数
    for (int j = MapY - 1; j >= 0; j--) {
        if (ChessMap[MapX][j] == chess)
            num++;
        else break;
    }
    // 向右计数
    for (int j = MapY + 1; j < N; j++) {
        if (ChessMap[MapX][j] == chess)
            num++;
        else break;
    }
    //判断水平方向总连珠数是否满足获胜条件（>=5）
    if (num >= 5)
        return chess;//返回获胜方


    /*3. 判断右上到左下方向*/
    num = 1;//重置计数变量
    // 向右上计数
    for (int i = MapX - 1, j = MapY + 1; i >= 0 && j < N; i--, j++) {
        if (ChessMap[i][j] == chess)
            num++;
        else break;
    }
    // 向左下检查
    for (int i = MapX + 1, j = MapY - 1; i < M && j >= 0; i++, j--) {
        if (ChessMap[i][j] == chess)
            num++;
        else break;
    }
    //判断右上到左下方向总连珠数是否满足获胜条件（>=5）
    if (num >= 5)
        return chess;//返回获胜方


    /*4. 判断左上到右下方向*/
    num = 1;//重置计数变量
    // 向左上检查
    for (int i = MapX - 1, j = MapY - 1; i >= 0 && j >= 0; i--, j--) {
        if (ChessMap[i][j] == chess)
            num++;
        else break;
    }
    // 向右下检查
    for (int i = MapX + 1, j = MapY + 1; i < M && j < N; i++, j++) {
        if (ChessMap[i][j] == chess)
            num++;
        else break;
    }
    //判断左上到右下方向总连珠数是否满足获胜条件（>=5）
    if (num >= 5)
        return chess;

    return 0;  // 未分出胜负
}

// 评估当前位置的得分（简单评估函数）
int evaluatePosition(int x, int y, int color) {
    if (board[x][y] != 0)
        return -1;  // 此位置已有棋子，不需要对该位置进行评估

    //定义计分变量
    int score = 0;

    // 中心区域加分
    if (x >= M / 2 - 4 && x <= M / 2 + 4 && y >= N / 2 - 4 && y <= N / 2 + 4)
        score += 5;

    // 检查周围是否有同色棋子
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < M && ny >= 0 && ny < N && board[nx][ny] == color)
                score += 3;
        }
    }

    return score;
}

// AI选择落子位置
void aiMakeMove(int* x, int* y, int color) {
    // 根据难度选择不同的AI模式
    if (aiDifficulty == AI_EASY) {
        // 简单模式：随机选择空位落子
        int emptyCount = 0;
        int emptyPositions[M * N][2];

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == 0) {
                    emptyPositions[emptyCount][0] = i;
                    emptyPositions[emptyCount][1] = j;
                    emptyCount++;
                }
            }
        }

        if (emptyCount > 0) {
            int index = rand() % emptyCount;
            *x = emptyPositions[index][0];
            *y = emptyPositions[index][1];
        }
        return;
    }

    // 普通和困难模式：根据评估函数选择位置
    int bestX = M / 2, bestY = N / 2;  // 默认选择棋盘中心位置
    int bestScore = -1;

    // 遍历整个棋盘寻找空位，并计算得分
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == 0) {
                int score = evaluatePosition(i, j, color);

                // 若为困难模式，还会阻碍对手完成五子连线
                if (aiDifficulty == AI_HARD) {
                    int opponentScore = evaluatePosition(i, j, 3 - color);
                    score = score * 2 + opponentScore;
                }

                // 更新最佳位置
                if (score > bestScore) {
                    bestScore = score;
                    bestX = i;
                    bestY = j;
                }
            }
        }
    }

    *x = bestX;
    *y = bestY;
}

// 处理AI回合
void processAITurn(int color) {
    // 通过延迟让AI看起来在思考
    Sleep(300);

    int aiX, aiY;
    aiMakeMove(&aiX, &aiY, color);

    // 再次检查该位置是否为空
    if (board[aiX][aiY] != 0) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == 0) {
                    aiX = i;
                    aiY = j;
                    break;
                }
            }
        }
    }

    // AI落子并渲染棋子
    if (color == 1) {
        setfillcolor(BLACK);
        board[aiX][aiY] = 1;
    }
    else {
        setfillcolor(WHITE);
        board[aiX][aiY] = 2;
    }

    Put_animation(aiX, aiY, color);

    setcolor(RED);

    // 判断游戏结果
    int result = judge(board, aiX, aiY);
    if (result == color) {
        if (color == 1) {
            outtextxy(515, 300, "黑棋(AI)获胜!");
        }
        else {
            outtextxy(515, 300, "白棋(AI)获胜!");
        }
        _getch();
        exit(0);
    }
}

// 显示游戏菜单
void showMenu() {
    // 加载背景图片
    loadimage(NULL, "ChessUnderground.jpg");

    // 在右侧绘制黑色矩形，用来显示选项文字
    setfillcolor(BLACK);
    solidrectangle(500, 0, 700, 500);

    // 显示菜单选项
    settextcolor(WHITE);
    outtextxy(515, 30, "五子棋游戏");

    // 游戏模式选择
    outtextxy(515, 70, "游戏模式:");
    outtextxy(530, 95, "1. 玩家 vs 玩家");
    outtextxy(530, 120, "2. 玩家 vs AI");
    outtextxy(530, 145, "3. AI 演示模式");

    // AI难度选择
    outtextxy(515, 175, "AI难度:");
    outtextxy(530, 200, "1. 简单");
    outtextxy(530, 225, "2. 普通");
    outtextxy(530, 250, "3. 困难");

    // 执棋颜色选择
    outtextxy(515, 280, "AI执棋:");
    outtextxy(530, 305, "1. AI执黑(先手)");
    outtextxy(530, 330, "2. AI执白(后手)");

    outtextxy(515, 370, "按S开始游戏");
    outtextxy(515, 395, "按ESC退出");

    // 时间限制功能选择
    outtextxy(515, 435, "时间限制:");
    char timeLimitSet[50];
    sprintf_s(timeLimitSet, "%s", timeLimitEnabled == 1 ? "T. 已启用" : "T. 已禁用");
    outtextxy(530, 460, timeLimitSet);
}

// 处理菜单输入
int handleMenuInput() {
    MOUSEMSG msg;

    // 使用死循环监听用户的鼠标点击和键盘输入
    while (1) {
        if (MouseHit()) {
            msg = GetMouseMsg();

            // 检测鼠标左键点击区域
            if (msg.uMsg == WM_LBUTTONDOWN) {
                int x = msg.x;
                int y = msg.y;

                // 游戏模式选择
                if (x >= 530 && x <= 650) {
                    if (y >= 95 && y <= 110) {
                        gameMode = MODE_PVP;
                        return 1;
                    }
                    else if (y >= 120 && y <= 135) {
                        gameMode = MODE_PVE;
                        return 1;
                    }
                    else if (y >= 145 && y <= 160) {
                        gameMode = MODE_AI_ONLY;
                        return 1;
                    }

                    // AI难度选择
                    if (y >= 200 && y <= 215) {
                        aiDifficulty = AI_EASY;
                    }
                    else if (y >= 225 && y <= 240) {
                        aiDifficulty = AI_NORMAL;
                    }
                    else if (y >= 250 && y <= 265) {
                        aiDifficulty = AI_HARD;
                    }

                    // AI执棋选择
                    if (y >= 305 && y <= 320) {   // AI执黑
                        aiTurn = 0;
                    }
                    else if (y >= 330 && y <= 345) {   // AI执白
                        aiTurn = 1;
                    }
                    if (y >= 435 && y <= 450) {
                        timeLimitEnabled = !timeLimitEnabled; // 切换时间开关状态
                        setfillcolor(BLACK);
                        solidrectangle(530, 460, 700, 490);
                        char timeLimitSet[50];
                        sprintf_s(timeLimitSet, "%s", timeLimitEnabled == 1 ? "T. 已启用" : "T. 已禁用");
                        outtextxy(530, 460, timeLimitSet);
                    }
                }

                // 点击“按S开始游戏”按钮也可开始游戏
                if (x >= 515 && x <= 600 && y >= 370 && y <= 385) {
                    return 1;
                }
            }
        }

        // 监听键盘输入（大小写均可）
        if (_kbhit()) {
            int key = _getch();
            if (key == 's' || key == 'S') {
                return 1;
            }
            else if (key == 27) {
                return 0;
            }
            if (key == 't' || key == 'T') {
                timeLimitEnabled = !timeLimitEnabled;
                setfillcolor(BLACK);
                solidrectangle(530, 460, 700, 490);
                char timeLimitSet[50];
                sprintf_s(timeLimitSet, "%s", timeLimitEnabled == 1 ? "T. 已启用" : "T. 已禁用");
                outtextxy(530, 460, timeLimitSet);
            }
        }
    }
}

// 重置游戏
void resetGame() {
    // 所有位置设为空，清空棋盘
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            board[i][j] = 0;
        }
    }

    // 重置当前玩家，当前回合设为黑棋先手
    currentPlayer = 1;

    // 重新绘制棋盘
    cleardevice();
    initGame();
}

// 将鼠标坐标转换为棋盘坐标
bool convertMouseToBoard(int mouseX, int mouseY, int* boardX, int* boardY) {
    // 计算最接近的棋盘交叉点
    int closestX = (mouseX + CELL_SIZE / 2) / CELL_SIZE;
    int closestY = (mouseY + CELL_SIZE / 2) / CELL_SIZE;

    // 计算实际交叉点坐标
    int actualX = closestX * CELL_SIZE;
    int actualY = closestY * CELL_SIZE;

    // 设置点击容错，允许有10像素的误差范围
    if (abs(mouseX - actualX) <= 10 && abs(mouseY - actualY) <= 10) {
        *boardX = closestY - 1;
        *boardY = closestX - 1;

        // 检查坐标是否在棋盘范围内
        if (*boardX >= 0 && *boardX < M && *boardY >= 0 && *boardY < N) {
            return true;
        }
    }
    return false;
}

// 主游戏循环
void playGame() {
    MOUSEMSG Msg;


    srand(time(NULL));//通过初始化随机种子使得每一次的AI落子位置随机


    if (aiTurn == 0 && (gameMode == MODE_PVE || gameMode == MODE_AI_ONLY) && currentPlayer == 1)
    {
        processAITurn(1);
        currentPlayer = 2;
        startTime = GetTickCount();
    }

    while (1) //使游戏无限循环直到分出胜负或者退出游戏
    {
        if (gameMode != MODE_AI_ONLY)//判断是否AI回合，AI回合不需要计时
        {
            bool isAiTurn = false;
            if (gameMode == MODE_PVE)
            {
                if ((aiTurn == 0 && currentPlayer == 1) || (aiTurn == 1 && currentPlayer == 2))
                {
                    isAiTurn = true;
                }
            }


            if (!isAiTurn)
            {
                unsigned long now = GetTickCount();
                timeLeft = stepTimeLimit - (now - startTime) / 1000;
                if (timeLimitEnabled && timeLeft <= 0)
                {
                    MessageBox(NULL, "您已超时！已切换另一位玩家落子！", "提示", MB_OK);


                    currentPlayer = (currentPlayer == 1) ? 2 : 1;//实现玩家的切换
                    startTime = GetTickCount();

                    if (gameMode == MODE_PVE && ((aiTurn == 0 && currentPlayer == 1) || (aiTurn == 1 && currentPlayer == 2)))
                    {
                        processAITurn(currentPlayer);
                        currentPlayer = 3 - currentPlayer;
                        startTime = GetTickCount();
                        continue;
                    }

                    setfillcolor(WHITE);
                    solidrectangle(15, 550, 250, 580);
                    DrawTime();
                    continue;
                }
                if (timeLimitEnabled)
                {

                    setfillcolor(WHITE);
                    solidrectangle(15, 550, 250, 580);
                    DrawTime();
                }
            }
        }
        if (gameMode == MODE_AI_ONLY) // AI演示模式
        {
            processAITurn(currentPlayer);
            currentPlayer = 3 - currentPlayer;
            continue;
        }


        if (gameMode == MODE_PVE && ((aiTurn == 0 && currentPlayer == 1) || (aiTurn == 1 && currentPlayer == 2))) {
            processAITurn(currentPlayer);
            currentPlayer = 3 - currentPlayer;
            startTime = GetTickCount();
            continue;
        }

        // 玩家回合
        if (MouseHit()) {//检测是否有鼠标消息的点击和移动
            Msg = GetMouseMsg();
            if (Msg.uMsg == WM_LBUTTONDOWN) {//只响应左键
                int boardX, boardY;
                if (convertMouseToBoard(Msg.x, Msg.y, &boardX, &boardY)) {
                    if (board[boardX][boardY] == 0) {
                        if (currentPlayer == 1) {
                            Put_animation(boardX, boardY, currentPlayer);
                            setcolor(RED);
                            board[boardX][boardY] = 1;
                            int result = judge(board, boardX, boardY);
                            if (result == 1) {
                                outtextxy(530, 300, "黑棋获胜!");
                                _getch();
                                return;
                            }
                            currentPlayer = 2;
                        }

                        else if (currentPlayer == 2) {
                            Put_animation(boardX, boardY, currentPlayer);
                            setcolor(RED);
                            board[boardX][boardY] = 2;
                            int result = judge(board, boardX, boardY);
                            if (result == 2) {
                                outtextxy(530, 300, "白棋获胜!");
                                _getch();
                                return;
                            }
                            currentPlayer = 1;
                        }

                        startTime = GetTickCount();
                        setbkmode(TRANSPARENT);
                        settextcolor(BLACK);
                        setfillcolor(WHITE);
                        solidrectangle(530, 180, 700, 210);
                        char playerText[50];
                        sprintf_s(playerText, "当前回合: %s", currentPlayer == 1 ? "黑棋" : "白棋");
                        outtextxy(530, 180, playerText);
                    }
                }
            }
        }
        if (_kbhit()) {//检测是否有键盘按键
            int key = _getch();
            if (key == 'r' || key == 'R') {
                resetGame();
                currentPlayer = 1;
                if (aiTurn == 0 && (gameMode == MODE_PVE || gameMode == MODE_AI_ONLY)) {
                    processAITurn(1);
                    currentPlayer = 2;
                }
            }
            else if (key == 27) {
                return;
            }
        }
    }
}


// 主函数
int main() {
    // 显示菜单
    initgraph(700, 500);
    showMenu();

    // 处理菜单选择
    if (handleMenuInput()) {
        closegraph();  // 关闭菜单

        // 初始化游戏界面
        initGame();

        // 开始游戏
        playGame();
    }

    closegraph();  // 关闭窗口
    return 0;
}