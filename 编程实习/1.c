#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

// 枚举定义
enum Element {
    SPACE = 0,   // 空地
    WALL = 1,    // 墙
    DEST = 2,    // 目的地
    BOX = 3,     // 箱子
    PLAYER = 4   // 玩家
};

// 全局变量
int level = 0;           // 当前关卡（0 ~ 2）
int steps = 0;           // 当前关卡步数
int totalSteps = 0;      // 总步数
int totalLevels = 3;     // 总关卡数
int levelSteps[3] = {0}; // 每关步数记录

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

// 控制台句柄（用于定位输出）
HANDLE hConsole = NULL;

// 初始化控制台（定位光标 + 隐藏光标）
void initConsole() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // 隐藏光标
    CONSOLE_CURSOR_INFO cursorInfo = {1, 0};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// 定位光标到指定坐标（适配中文符号）
void gotoXY(int x, int y) {
    COORD pos = {x * 2, y + 4};  // x*2 处理全角字符宽度
    SetConsoleCursorPosition(hConsole, pos);
}

// 更新步数显示
void updateStepDisplay() {
    // 定位到步数显示行
    COORD pos = {0, 1};
    SetConsoleCursorPosition(hConsole, pos);
    printf("  关卡: %d / %d    当前步数: %d    总步数: %d    ", 
           level + 1, totalLevels, steps, totalSteps);
    // 清除行末可能的残留字符
    printf("        ");
}

// 绘制单个地图格子（局部重绘）
void drawCell(int row, int col) {
    gotoXY(col, row);
    switch (map[level][row][col]) {
        case SPACE:     printf("  "); break;
        case WALL:      printf("■ "); break;
        case DEST:      printf("☆ "); break;
        case BOX:       printf("□ "); break;
        case PLAYER:    printf("♀ "); break;
        case PLAYER + DEST: printf("♂ "); break;
        case BOX + DEST:  printf("★ "); break;
    }
}

// 绘制固定 UI（标题、关卡信息）
void drawStaticUI() {
    system("cls");
    printf("================ 推箱子游戏 ================\n");
    updateStepDisplay();
    printf("  操作: W(上) A(左) S(下) D(右) | R(重置) | Q(退出)\n");
    printf("============================================\n\n");
}

// 移动逻辑（返回是否移动，用于局部重绘）
bool handleMove(int *playerRow, int *playerCol, 
                int newRow, int newCol, 
                int boxRow, int boxCol) {
    if (newRow < 0 || newRow >= 10 || newCol < 0 || newCol >= 10) {
        return false;
    }

    int target = map[level][newRow][newCol];
    bool moved = false;

    // 情况 1：移动玩家（空地/目的地）
    if (target == SPACE || target == DEST) {
        map[level][*playerRow][*playerCol] -= PLAYER;
        map[level][newRow][newCol] += PLAYER;
        drawCell(*playerRow, *playerCol); // 重绘原位置
        *playerRow = newRow;
        *playerCol = newCol;
        drawCell(*playerRow, *playerCol); // 重绘新位置
        moved = true;
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
        drawCell(newRow, newCol);   // 箱子原位置
        drawCell(boxRow, boxCol);   // 箱子新位置

        // 移动玩家
        map[level][*playerRow][*playerCol] -= PLAYER;
        map[level][newRow][newCol] += PLAYER;
        drawCell(*playerRow, *playerCol); // 玩家原位置
        *playerRow = newRow;
        *playerCol = newCol;
        drawCell(*playerRow, *playerCol); // 玩家新位置
        moved = true;
    }

    if (moved) {
        steps++;
        updateStepDisplay(); // 更新步数显示
    }
    return moved;
}

// 处理键盘事件（用 int 存储按键，解决类型警告）
void keyevent(int *playerRow, int *playerCol) {
    if (_kbhit()) {
        int ch = _getch(); // 用 int 存储，避免 char 范围问题
        
        // 处理方向键（二次读取）
        if (ch == 0 || ch == 224) {
            ch = _getch(); // 读取实际方向键编码
        }

        switch (ch) {
            case 'q': case 'Q': 
                exit(0); 
                break;
            case 'r': case 'R': { // 重置关卡
                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 10; j++)
                        map[level][i][j] = map[level][i][j] % 4;
                *playerRow = playerInitPos[level][0];
                *playerCol = playerInitPos[level][1];
                map[level][*playerRow][*playerCol] = PLAYER;
                steps = 0;
                drawStaticUI(); // 重置后重绘全屏
                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 10; j++)
                        drawCell(i, j);
                break;
            }
            // 方向键处理（上、下、左、右）
            case 'w': case 'W': case 72: // 上（ASCII 72）
                handleMove(playerRow, playerCol, 
                          *playerRow-1, *playerCol, 
                          *playerRow-2, *playerCol);
                break;
            case 's': case 'S': case 80: // 下（ASCII 80）
                handleMove(playerRow, playerCol, 
                          *playerRow+1, *playerCol, 
                          *playerRow+2, *playerCol);
                break;
            case 'a': case 'A': case 75: // 左（ASCII 75）
                handleMove(playerRow, playerCol, 
                          *playerRow, *playerCol-1, 
                          *playerRow, *playerCol-2);
                break;
            case 'd': case 'D': case 77: // 右（ASCII 77）
                handleMove(playerRow, playerCol, 
                          *playerRow, *playerCol+1, 
                          *playerRow, *playerCol+2);
                break;
        }
    }
}

// 判断是否通关
bool jude() {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            if (map[level][i][j] == BOX)
                return false;
    return true;
}

int main() {
    system("chcp 65001 > nul");  // 中文编码
    system("mode con cols=30 lines=20");
    initConsole();

    int playerRow = playerInitPos[level][0];
    int playerCol = playerInitPos[level][1];

    drawStaticUI();
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            drawCell(i, j);

    while (1) {
        keyevent(&playerRow, &playerCol); // 异步处理按键

        if (jude()) {
            // 记录当前关卡步数
            levelSteps[level] = steps;
            totalSteps += steps;
            
            system("cls");
            printf("================ 通关提示 ================\n");
            printf("  恭喜通过第 %d 关！\n", level + 1);
            printf("  本关步数: %d 步\n", steps);
            printf("  总步数: %d 步\n\n", totalSteps);
            printf("  按任意键进入下一关...");
            _getch();

            level++;
            steps = 0;
            if (level >= totalLevels) {
                system("cls");
                printf("================ 通关庆祝 ================\n");
                printf("  oioioioioioioioi 恭喜通过所有关卡！\n\n");
                printf("  各关卡步数统计:\n");
                for (int i = 0; i < totalLevels; i++) {
                    printf("  第 %d 关: %d 步\n", i + 1, levelSteps[i]);
                }
                printf("\n  总步数: %d 步\n\n", totalSteps);
                printf("  按任意键退出游戏...");
                _getch();
                break;
            }

            // 初始化下一关
            playerRow = playerInitPos[level][0];
            playerCol = playerInitPos[level][1];
            map[level][playerRow][playerCol] = PLAYER;
            drawStaticUI();
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 10; j++)
                    drawCell(i, j);
        }
    }

    return 0;
}