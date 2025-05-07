#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// 枚举定义
enum Element {
    SPACE = 0,   // 空地
    WALL = 1,    // 墙
    DEST = 2,    // 目的地
    BOX = 3,     // 箱子
    PLAYER = 4   // 玩家
};

// 颜色代码
enum Colors {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,
    GRAY = 8,
    BRIGHT_BLUE = 9,
    BRIGHT_GREEN = 10,
    BRIGHT_CYAN = 11,
    BRIGHT_RED = 12,
    BRIGHT_MAGENTA = 13,
    BRIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
};

// 全局变量
int level = 0;           // 当前关卡（0 ~ 2）
int steps = 0;           // 当前关卡步数
int totalSteps = 0;      // 总步数
int totalLevels = 3;     // 总关卡数
int levelSteps[3] = {0}; // 每关步数记录
int levelTime[3] = {0};  // 每关用时记录
time_t levelStartTime;   // 关卡开始时间
int levelTimeLimit[3] = {120, 180, 240}; // 关卡时间限制（秒）
int levelStepLimit[3] = {100, 150, 200}; // 关卡步数限制
int starRating[3] = {0}; // 每关星级评价
bool soundEnabled = true;// 音效开关

// 地图数据（3 关 × 10 行 × 10 列）
int map[3][10][10] = {
    // 关卡 1
    {
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,1,1,1,1,1,0,0},
        {0,0,0,1,2,0,0,1,1,0},
        {0,1,1,1,3,0,3,2,1,0},
        {0,1,2,3,4,0,1,1,1,0},
        {0,1,1,1,1,3,1,0,0,0},
        {0,0,0,0,1,2,1,0,0,0},
        {0,0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0}
    },
    // 关卡 2
    {
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,1,1,0,0,1,1,0,0},
        {0,1,0,2,1,1,2,0,1,0},
        {1,0,0,0,3,0,0,0,0,1},
        {1,0,0,0,4,3,0,0,0,1},
        {0,1,0,0,3,3,0,0,1,0},
        {0,0,1,0,0,0,0,1,0,0},
        {0,0,0,1,2,2,1,0,0,0},
        {0,0,0,0,1,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0}
    },
    // 关卡 3
    {
        {0,0,0,0,1,0,0,0,0,0},
        {0,0,0,1,0,1,0,0,0,0},
        {0,0,1,2,3,0,1,0,0,0},
        {0,1,0,0,0,0,0,1,0,0},
        {1,2,3,0,4,0,0,0,1,0},
        {0,1,0,0,0,0,0,3,2,1},
        {0,0,1,0,3,0,0,0,1,0},
        {0,0,0,1,2,0,0,1,0,0},
        {0,0,0,0,1,0,1,0,0,0},
        {0,0,0,0,0,1,0,0,0,0}
    }
};

// 玩家初始位置
int playerInitPos[3][2] = { {4,4}, {4,4}, {4,4} };

// 控制台句柄
HANDLE hConsole = NULL;

// 设置控制台颜色
void setColor(int bg, int fg) {
    SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

// 播放音效
void playSound(int type) {
    if (!soundEnabled) return;
    
    switch (type) {
        case 1: // 移动
            Beep(400, 50);
            break;
        case 2: // 推箱子
            Beep(600, 60);
            break;
        case 3: // 成功
            Beep(784, 100);
            Beep(1046, 200);
            break;
        case 4: // 失败
            Beep(523, 100);
            Beep(392, 200);
            break;
        case 5: // 重置
            Beep(330, 80);
            Beep(440, 80);
            break;
    }
}

// 初始化控制台
void initConsole() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // 隐藏光标
    CONSOLE_CURSOR_INFO cursorInfo = {1, 0};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// 定位光标
void gotoXY(int x, int y) {
    COORD pos = {x * 2, y + 4};  // x*2 处理全角字符宽度
    SetConsoleCursorPosition(hConsole, pos);
}

// 更新步数和时间显示
void updateDisplay() {
    // 定位到显示行
    COORD pos = {0, 1};
    SetConsoleCursorPosition(hConsole, pos);
    
    // 计算当前用时
    int elapsedTime = (int)difftime(time(NULL), levelStartTime);
    
    setColor(BLUE, BRIGHT_WHITE);
    printf(" 关卡: %d/%d ", level + 1, totalLevels);
    
    setColor(BLACK, BRIGHT_WHITE);
    printf(" 步数: ");
    if (steps > levelStepLimit[level] * 0.8) {
        setColor(BLACK, BRIGHT_RED); // 接近限制时变红
    } else {
        setColor(BLACK, BRIGHT_YELLOW);
    }
    printf("%d/%d", steps, levelStepLimit[level]);
    
    setColor(BLACK, BRIGHT_WHITE);
    printf(" 时间: ");
    if (elapsedTime > levelTimeLimit[level] * 0.8) {
        setColor(BLACK, BRIGHT_RED);
    } else {
        setColor(BLACK, BRIGHT_GREEN);
    }
    printf("%d/%d秒", elapsedTime, levelTimeLimit[level]);
    
    setColor(BLACK, BRIGHT_WHITE);
    printf(" 星级: ");
    setColor(BLACK, BRIGHT_YELLOW);
    for (int i = 0; i < 3; i++) {
        if (i < starRating[level]) {
            printf("★");
        } else {
            printf("☆");
        }
    }
    
    // 清除行末可能的残留字符
    setColor(BLACK, BLACK);
    printf("        ");
}

// 绘制单个地图格子
void drawCell(int row, int col) {
    gotoXY(col, row);
    
    int cellValue = map[level][row][col];
    int isOnDest = (cellValue == PLAYER + DEST || cellValue == BOX + DEST);
    
    switch (cellValue) {
        case SPACE:
            setColor(BLACK, BLACK);
            printf("  ");
            break;
        case WALL:
            setColor(BLUE, BRIGHT_WHITE);
            printf("■ ");
            break;
        case DEST:
            setColor(BLACK, BRIGHT_MAGENTA);
            printf("◎ ");
            break;
        case BOX:
            setColor(BLACK, BRIGHT_YELLOW);
            printf("■ ");
            break;
        case PLAYER:
            setColor(BLACK, BRIGHT_GREEN);
            printf("☺ ");
            break;
        case PLAYER + DEST:
            setColor(BLACK, BRIGHT_GREEN);
            printf("☻ ");
            break;
        case BOX + DEST:
            setColor(BLACK, BRIGHT_RED);
            printf("■ ");
            break;
    }
}

// 绘制游戏标题
void drawTitle() {
    setColor(BLUE, BRIGHT_WHITE);
    printf("\n");
    printf("  ██████╗ ███████╗███████╗███████╗██╗  ██╗\n");
    printf("  ██╔══██╗██╔════╝██╔════╝██╔════╝╚██╗██╔╝\n");
    printf("  ██████╔╝█████╗  ███████╗█████╗   ╚███╔╝ \n");
    printf("  ██╔══██╗██╔══╝  ╚════██║██╔══╝   ██╔██╗ \n");
    printf("  ██║  ██║███████╗███████║███████╗██╔╝ ██╗\n");
    printf("  ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝╚═╝  ╚═╝\n");
    printf("         推 箱 子 大 冒 险 - 增强版\n");
    setColor(BLACK, BRIGHT_WHITE);
    printf("============================================\n");
}

// 绘制固定 UI
void drawStaticUI() {
    system("cls");
    drawTitle();
    
    // 显示操作说明
    setColor(BLACK, BRIGHT_WHITE);
    gotoXY(0, 15);
    printf("操作说明:");
    setColor(BLACK, BRIGHT_GREEN);
    printf(" 方向键:移动");
    setColor(BLACK, BRIGHT_YELLOW);
    printf(" R:重置");
    setColor(BLACK, BRIGHT_CYAN);
    printf(" N:下一关");
    setColor(BLACK, BRIGHT_MAGENTA);
    printf(" P:上一关");
    setColor(BLACK, BRIGHT_RED);
    printf(" Q:退出");
    setColor(BLACK, BRIGHT_WHITE);
    printf(" T:音效");
    
    // 绘制地图边框
    setColor(BLUE, BRIGHT_WHITE);
    for (int i = -1; i <= 10; i++) {
        gotoXY(i, -1);
        printf("▄▄");
        gotoXY(i, 10);
        printf("▀▀");
    }
    for (int i = -1; i <= 10; i++) {
        gotoXY(-1, i);
        printf("█");
        gotoXY(10, i);
        printf("█");
    }
}

// 移动逻辑
bool handleMove(int *playerRow, int *playerCol, 
                int newRow, int newCol, 
                int boxRow, int boxCol) {
    if (newRow < 0 || newRow >= 10 || newCol < 0 || newCol >= 10) {
        return false;
    }

    int target = map[level][newRow][newCol];
    bool moved = false;

    // 情况 1：移动玩家
    if (target == SPACE || target == DEST) {
        map[level][*playerRow][*playerCol] -= PLAYER;
        map[level][newRow][newCol] += PLAYER;
        drawCell(*playerRow, *playerCol);
        *playerRow = newRow;
        *playerCol = newCol;
        drawCell(*playerRow, *playerCol);
        moved = true;
        playSound(1);
    }
    // 情况 2：推动箱子
    else if (target == BOX || target == BOX + DEST) {
        if (boxRow < 0 || boxRow >= 10 || boxCol < 0 || boxCol >= 10) {
            return false;
        }
        int boxTarget = map[level][boxRow][boxCol];
        if (boxTarget == WALL || boxTarget == BOX || boxTarget == BOX + DEST) {
            return false;
        }

        // 移动箱子
        map[level][boxRow][boxCol] += BOX;
        map[level][newRow][newCol] -= BOX;
        drawCell(newRow, newCol);
        drawCell(boxRow, boxCol);

        // 移动玩家
        map[level][*playerRow][*playerCol] -= PLAYER;
        map[level][newRow][newCol] += PLAYER;
        drawCell(*playerRow, *playerCol);
        *playerRow = newRow;
        *playerCol = newCol;
        drawCell(*playerRow, *playerCol);
        moved = true;
        playSound(2);
    }

    if (moved) {
        steps++;
        updateDisplay();
    }
    return moved;
}

// 处理键盘事件
// 处理键盘事件
void keyevent(int *playerRow, int *playerCol) {
    if (_kbhit()) {
        int ch = _getch();
        
        // 处理方向键
        if (ch == 0 || ch == 224) {
            ch = _getch();
            // 只处理方向键，不处理字母键
            switch (ch) {
                case 72: // 上
                    handleMove(playerRow, playerCol, 
                              *playerRow-1, *playerCol, 
                              *playerRow-2, *playerCol);
                    break;
                case 80: // 下
                    handleMove(playerRow, playerCol, 
                              *playerRow+1, *playerCol, 
                              *playerRow+2, *playerCol);
                    break;
                case 75: // 左
                    handleMove(playerRow, playerCol, 
                              *playerRow, *playerCol-1, 
                              *playerRow, *playerCol-2);
                    break;
                case 77: // 右
                    handleMove(playerRow, playerCol, 
                              *playerRow, *playerCol+1, 
                              *playerRow, *playerCol+2);
                    break;
            }
        } else {
            // 处理普通按键（字母键）
            switch (tolower(ch)) {  // 转换为小写统一处理
                case 'q': 
                    exit(0); 
                    break;
                    
                case 't': // 音效开关
                    soundEnabled = !soundEnabled;
                    setColor(BLACK, BRIGHT_WHITE);
                    gotoXY(0, 16);
                    printf("音效: %s", soundEnabled ? "开启  " : "关闭  ");
                    playSound(5);
                    break;
                    
                case 'r': // 重置关卡
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < 10; j++) {
                            map[level][i][j] = map[level][i][j] % 4;
                        }
                    }
                    *playerRow = playerInitPos[level][0];
                    *playerCol = playerInitPos[level][1];
                    map[level][*playerRow][*playerCol] = PLAYER;
                    steps = 0;
                    levelStartTime = time(NULL);
                    drawStaticUI();
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < 10; j++) {
                            drawCell(i, j);
                        }
                    }
                    updateDisplay();
                    playSound(5);
                    break;
                    
                case 'n': // 下一关
                    if (level < totalLevels - 1) {
                        level++;
                        *playerRow = playerInitPos[level][0];
                        *playerCol = playerInitPos[level][1];
                        map[level][*playerRow][*playerCol] = PLAYER;
                        steps = 0;
                        levelStartTime = time(NULL);
                        drawStaticUI();
                        for (int i = 0; i < 10; i++) {
                            for (int j = 0; j < 10; j++) {
                                drawCell(i, j);
                            }
                        }
                        updateDisplay();
                        playSound(3);
                    }
                    break;
                    
                case 'p': // 上一关
                    if (level > 0) {
                        level--;
                        *playerRow = playerInitPos[level][0];
                        *playerCol = playerInitPos[level][1];
                        map[level][*playerRow][*playerCol] = PLAYER;
                        steps = 0;
                        levelStartTime = time(NULL);
                        drawStaticUI();
                        for (int i = 0; i < 10; i++) {
                            for (int j = 0; j < 10; j++) {
                                drawCell(i, j);
                            }
                        }
                        updateDisplay();
                        playSound(3);
                    }
                    break;
                    
                // 字母方向控制
                case 'w': case 'k': // 上
                    handleMove(playerRow, playerCol, 
                              *playerRow-1, *playerCol, 
                              *playerRow-2, *playerCol);
                    break;
                case 's': case 'j': // 下
                    handleMove(playerRow, playerCol, 
                              *playerRow+1, *playerCol, 
                              *playerRow+2, *playerCol);
                    break;
                case 'a': case 'h': // 左
                    handleMove(playerRow, playerCol, 
                              *playerRow, *playerCol-1, 
                              *playerRow, *playerCol-2);
                    break;
                case 'd': case 'l': // 右
                    handleMove(playerRow, playerCol, 
                              *playerRow, *playerCol+1, 
                              *playerRow, *playerCol+2);
                    break;
            }
        }
    }
}

// 判断是否通关
bool isLevelComplete() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (map[level][i][j] == DEST || map[level][i][j] == PLAYER + DEST) {
                return false;
            }
        }
    }
    return true;
}

// 计算星级评价
int calculateStarRating() {
    int elapsedTime = (int)difftime(time(NULL), levelStartTime);
    int timeScore = (levelTimeLimit[level] - elapsedTime) * 100 / levelTimeLimit[level];
    int stepScore = (levelStepLimit[level] - steps) * 100 / levelStepLimit[level];
    int totalScore = (timeScore + stepScore) / 2;
    
    if (totalScore > 80) return 3;
    if (totalScore > 50) return 2;
    if (totalScore > 30) return 1;
    return 0;
}

// 显示通关信息
void showLevelComplete() {
    int elapsedTime = (int)difftime(time(NULL), levelStartTime);
    levelTime[level] = elapsedTime;
    levelSteps[level] = steps;
    starRating[level] = calculateStarRating();
    totalSteps += steps;
    
    system("cls");
    
    // 绘制庆祝界面
    setColor(BLUE, BRIGHT_WHITE);
    printf("\n");
    printf("  ██████╗  ██████╗ ██████╗ ███████╗██████╗ \n");
    printf("  ██╔════╝ ██╔═══██╗██╔══██╗██╔════╝██╔══██╗\n");
    printf("  ██║  ███╗██║   ██║██████╔╝█████╗  ██████╔╝\n");
    printf("  ██║   ██║██║   ██║██╔═══╝ ██╔══╝  ██╔══██╗\n");
    printf("  ╚██████╔╝╚██████╔╝██║     ███████╗██║  ██║\n");
    printf("   ╚═════╝  ╚═════╝ ╚═╝     ╚══════╝╚═╝  ╚═╝\n");
    
    setColor(BLACK, BRIGHT_GREEN);
    printf("         恭喜通过第 %d 关！\n\n", level + 1);
    
    setColor(BLACK, BRIGHT_YELLOW);
    printf("         ★ 星级评价: ");
    for (int i = 0; i < 3; i++) {
        if (i < starRating[level]) {
            setColor(BLACK, BRIGHT_YELLOW);
            printf("★ ");
        } else {
            setColor(BLACK, GRAY);
            printf("☆ ");
        }
    }
    printf("\n\n");
    
    setColor(BLACK, BRIGHT_CYAN);
    printf("         用时: %d秒 (限制: %d秒)\n", elapsedTime, levelTimeLimit[level]);
    
    setColor(BLACK, BRIGHT_MAGENTA);
    printf("         步数: %d步 (限制: %d步)\n\n", steps, levelStepLimit[level]);
    
    setColor(BLACK, BRIGHT_WHITE);
    printf("         总步数: %d步\n\n", totalSteps);
    
    setColor(BLACK, BRIGHT_GREEN);
    printf("        按任意键继续...");
    playSound(3);
    _getch();
}

// 显示游戏结束信息
void showGameComplete() {
    system("cls");
    
    // 绘制通关庆祝界面
    setColor(BLUE, BRIGHT_WHITE);
    printf("\n");
    printf("  ██████╗ ██╗   ██╗███████╗██╗   ██╗██╗███╗   ██╗\n");
    printf("  ██╔══██╗██║   ██║██╔════╝██║   ██║██║████╗  ██║\n");
    printf("  ██████╔╝██║   ██║█████╗  ██║   ██║██║██╔██╗ ██║\n");
    printf("  ██╔═══╝ ██║   ██║██╔══╝  ╚██╗ ██╔╝██║██║╚██╗██║\n");
    printf("  ██║     ╚██████╔╝███████╗ ╚████╔╝ ██║██║ ╚████║\n");
    printf("  ╚═╝      ╚═════╝ ╚══════╝  ╚═══╝  ╚═╝╚═╝  ╚═══╝\n");
    
    setColor(BLACK, BRIGHT_YELLOW);
    printf("         恭喜你完成了所有关卡！\n\n");
    
    setColor(BLACK, BRIGHT_GREEN);
    printf("         最终总步数: %d步\n\n", totalSteps);
    
    setColor(BLACK, BRIGHT_CYAN);
    printf("         ===== 关卡评价 =====\n\n");
    
    for (int i = 0; i < totalLevels; i++) {
        setColor(BLACK, BRIGHT_MAGENTA);
        printf("         第%d关: ", i + 1);
        
        setColor(BLACK, BRIGHT_YELLOW);
        for (int j = 0; j < 3; j++) {
            if (j < starRating[i]) {
                printf("★");
            } else {
                printf("☆");
            }
        }
        
        setColor(BLACK, BRIGHT_CYAN);
        printf(" (步数:%d 用时:%d秒)\n", levelSteps[i], levelTime[i]);
    }
    
    setColor(BLACK, BRIGHT_WHITE);
    printf("\n        按任意键退出游戏...");
    playSound(3);
    _getch();
}

// 检查是否超时或超步数
bool isLevelFailed() {
    int elapsedTime = (int)difftime(time(NULL), levelStartTime);
    return (steps >= levelStepLimit[level] || elapsedTime >= levelTimeLimit[level]);
}

// 显示失败信息
void showLevelFailed() {
    system("cls");
    
    setColor(BLUE, BRIGHT_WHITE);
    printf("\n");
    printf("  ███████╗ █████╗ ██╗██╗     ██████╗ \n");
    printf("  ██╔════╝██╔══██╗██║██║     ██╔══██╗\n");
    printf("  █████╗  ███████║██║██║     ██║  ██║\n");
    printf("  ██╔══╝  ██╔══██║██║██║     ██║  ██║\n");
    printf("  ██║     ██║  ██║██║███████╗██████╔╝\n");
    printf("  ╚═╝     ╚═╝  ╚═╝╚═╝╚══════╝╚═════╝ \n");
    
    setColor(BLACK, BRIGHT_RED);
    printf("         第 %d 关挑战失败！\n\n", level + 1);
    
    setColor(BLACK, BRIGHT_YELLOW);
    printf("         失败原因: ");
    if (steps >= levelStepLimit[level]) {
        printf("超出步数限制 (%d/%d)\n", steps, levelStepLimit[level]);
    } else {
        int elapsedTime = (int)difftime(time(NULL), levelStartTime);
        printf("超出时间限制 (%d秒/%d秒)\n", elapsedTime, levelTimeLimit[level]);
    }
    
    setColor(BLACK, BRIGHT_WHITE);
    printf("\n         请按R键重新挑战本关");
    setColor(BLACK, BRIGHT_GREEN);
    printf("\n         或按其他键返回主菜单");
    
    playSound(4);
    int ch = _getch();
    if (ch == 'r' || ch == 'R') {
        // 重置关卡
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                map[level][i][j] = map[level][i][j] % 4;
            }
        }
        int playerRow = playerInitPos[level][0];
        int playerCol = playerInitPos[level][1];
        map[level][playerRow][playerCol] = PLAYER;
        steps = 0;
        levelStartTime = time(NULL);
        drawStaticUI();
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                drawCell(i, j);
            }
        }
        updateDisplay();
    } else {
        level = 0;
        steps = 0;
        totalSteps = 0;
        levelStartTime = time(NULL);
        drawStaticUI();
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                drawCell(i, j);
            }
        }
        updateDisplay();
    }
}

int main() {
    system("chcp 65001 > nul");  // 中文编码
    system("mode con cols=60 lines=25");
    initConsole();
    
    int playerRow = playerInitPos[level][0];
    int playerCol = playerInitPos[level][1];
    levelStartTime = time(NULL);
    
    // 初始化星级评价
    for (int i = 0; i < totalLevels; i++) {
        starRating[i] = 0;
    }
    
    drawStaticUI();
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            drawCell(i, j);
        }
    }
    updateDisplay();
    
    // 显示操作说明
    setColor(BLACK, BRIGHT_WHITE);
    gotoXY(0, 16);
    printf("音效: 开启");
    gotoXY(0, 17);
    printf("提示: 使用方向键或WASD移动，HJKL为Vim风格移动");
    
    while (1) {
        keyevent(&playerRow, &playerCol);
        
        if (isLevelComplete()) {
            showLevelComplete();
            
            level++;
            steps = 0;
            levelStartTime = time(NULL);
            
            if (level >= totalLevels) {
                showGameComplete();
                break;
            }
            
            // 初始化下一关
            playerRow = playerInitPos[level][0];
            playerCol = playerInitPos[level][1];
            map[level][playerRow][playerCol] = PLAYER;
            drawStaticUI();
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    drawCell(i, j);
                }
            }
            updateDisplay();
        } 
        else if (isLevelFailed()) {
            showLevelFailed();
            playerRow = playerInitPos[level][0];
            playerCol = playerInitPos[level][1];
        }
        
        // 更新显示（每0.5秒）
        static clock_t lastUpdate = 0;
        if (clock() - lastUpdate > 500) {
            updateDisplay();
            lastUpdate = clock();
        }
    }
    
    return 0;
}