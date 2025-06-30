#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>  // EasyX 图形库
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <wchar.h>  // 宽字符支持

enum Element { SPACE, WALL, DEST, BOX, PLAYER };

int level = 0, steps = 0, totalSteps = 0, totalLevels = 3;
int score = 0, totalScore = 0;  // 当前关卡得分和总得分
int levelSteps[3] = { 0 };
int levelScores[3] = { 0 };     // 记录每关得分
int entryTotalSteps = 0;        // 进入当前关卡时的总步数
int entryTotalScore = 0;        // 进入当前关卡时的总得分
const int CELL_SIZE = 40;

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

// 保存初始地图数据（用于重置）
int initialMap[3][10][10];
int playerInitPos[3][2] = { {4,4}, {4,4}, {4,4} };

IMAGE img_wall, img_player, img_box, img_dest, img_boxDest, img_playerDest, bg;
bool bgLoaded = false;

// 加载图片
bool loadImages() {
    const wchar_t* paths[] = {
        L"wall.png",
        L"player.png",
        L"box.png",
        L"dest.png",
        L"boxDest.png",
        L"playerDest.png",
        L"bg.png"
    };

    IMAGE* imgs[] = {
        &img_wall,
        &img_player,
        &img_box,
        &img_dest,
        &img_boxDest,
        &img_playerDest,
        &bg
    };

    for (int i = 0; i < 6; i++) {
        loadimage(imgs[i], paths[i], CELL_SIZE, CELL_SIZE, true);
    }

    loadimage(&bg, L"bg.png", 400, 400, true);
    bgLoaded = true;

    return true;
}

void initGraphics() {
    int winWidth = 10 * CELL_SIZE;
    int winHeight = 10 * CELL_SIZE + 100;  // 增加高度以显示得分
    initgraph(winWidth, winHeight);
    setbkcolor(WHITE);
    cleardevice();
}

// 初始化初始地图数据
void initInitialMap() {
    for (int l = 0; l < totalLevels; l++) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                initialMap[l][i][j] = map[l][i][j];
            }
        }
    }
}

// 更新步数和得分显示
void updateStepDisplay() {
    // 清除底部区域
    setfillcolor(WHITE);
    solidrectangle(0, 10 * CELL_SIZE, 10 * CELL_SIZE, 10 * CELL_SIZE + 100);

    // 显示步数
    settextcolor(RED);
    settextstyle(16, 0, _T("宋体"));
    wchar_t info[50];
    swprintf(info, _countof(info), L"关卡: %d/%d  步数: %d  总步数: %d",
        level + 1, totalLevels, steps, totalSteps);
    outtextxy(10, 10 * CELL_SIZE + 10, info);

    // 显示得分
    settextcolor(GREEN);
    swprintf(info, _countof(info), L"当前得分: %d  总得分: %d", score, totalScore);
    outtextxy(10, 10 * CELL_SIZE + 40, info);

    // 操作提示
    settextcolor(BLUE);
    settextstyle(14, 0, _T("宋体"));
    outtextxy(10, 10 * CELL_SIZE + 70, _T("操作: W(上) A(左) S(下) D(右) R(重置) Q(退出)"));
}

void drawCell(int row, int col) {
    int x = col * CELL_SIZE, y = row * CELL_SIZE;
    int element = map[level][row][col];

    switch (element) {
    case SPACE:
        if (bgLoaded) {
            putimage(x, y, CELL_SIZE, CELL_SIZE, &bg, x, y);
        }
        break;
    case WALL:    putimage(x, y, &img_wall); break;
    case DEST:    putimage(x, y, &img_dest); break;
    case BOX:     putimage(x, y, &img_box); break;
    case PLAYER:  putimage(x, y, &img_player); break;
    case PLAYER + DEST: putimage(x, y, &img_playerDest); break;
    case BOX + DEST:    putimage(x, y, &img_boxDest); break;
    }
}

void drawGame() {
    cleardevice();

    if (bgLoaded) {
        putimage(0, 0, &bg);
    }
    else {
        setbkcolor(LIGHTGRAY);
        solidrectangle(0, 0, 10 * CELL_SIZE, 10 * CELL_SIZE);
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            drawCell(i, j);
        }
    }

    updateStepDisplay();
    FlushBatchDraw(); // 刷新绘图
}

bool handleMove(int* playerRow, int* playerCol, int dirRow, int dirCol) {
    int originalRow = *playerRow;
    int originalCol = *playerCol;
    int newRow = *playerRow + dirRow;
    int newCol = *playerCol + dirCol;

    if (newRow < 0 || newRow >= 10 || newCol < 0 || newCol >= 10) {
        return false;
    }

    int target = map[level][newRow][newCol];
    bool moved = false;
    bool boxToDest = false;  // 记录箱子是否被推到目的地

    if (target == BOX || target == BOX + DEST) {
        int boxNewRow = newRow + dirRow;
        int boxNewCol = newCol + dirCol;
        if (boxNewRow < 0 || boxNewRow >= 10 || boxNewCol < 0 || boxNewCol >= 10) {
            return false;
        }
        int boxTarget = map[level][boxNewRow][boxNewCol];
        if (boxTarget == WALL || boxTarget == BOX || boxTarget == BOX + DEST) {
            return false;
        }

        // 检查箱子是否被推到目的地
        boxToDest = (boxTarget == DEST || boxTarget == DEST + PLAYER);

        map[level][boxNewRow][boxNewCol] += BOX;
        map[level][newRow][newCol] -= BOX;
        drawCell(newRow, newCol);
        drawCell(boxNewRow, boxNewCol);
    }

    if (target == SPACE || target == DEST || target == BOX || target == BOX + DEST) {
        int originalCell = map[level][originalRow][originalCol];
        if (originalCell == PLAYER) {
            map[level][originalRow][originalCol] = SPACE;
        }
        else if (originalCell == PLAYER + DEST) {
            map[level][originalRow][originalCol] = DEST;
        }
        drawCell(originalRow, originalCol);

        map[level][newRow][newCol] += PLAYER;
        *playerRow = newRow;
        *playerCol = newCol;
        drawCell(newRow, newCol);

        moved = true;
        steps++;
        totalSteps++;

        // 如果箱子被推到目的地，增加得分
        if (boxToDest) {
            score += 10;
            totalScore += 10;
        }

        drawGame(); // 移动后刷新界面，更新步数和得分
    }

    return moved;
}

void keyevent(int* playerRow, int* playerCol) {
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
        }

        switch (ch) {
        case 'q': case 'Q': exit(0); break;
        case 'r': case 'R': {
            // 重置当前关卡
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    map[level][i][j] = initialMap[level][i][j]; // 从初始地图恢复
                }
            }
            *playerRow = playerInitPos[level][0];
            *playerCol = playerInitPos[level][1];
            map[level][*playerRow][*playerCol] = PLAYER; // 恢复玩家位置
            steps = 0;
            score = 0;  // 重置关卡得分
            totalSteps = entryTotalSteps;  // 恢复总步数
            totalScore = entryTotalScore;  // 恢复总得分
            drawGame();
            break;
        }
        case 'w': case 'W': case 72: handleMove(playerRow, playerCol, -1, 0); break;
        case 's': case 'S': case 80: handleMove(playerRow, playerCol, 1, 0); break;
        case 'a': case 'A': case 75: handleMove(playerRow, playerCol, 0, -1); break;
        case 'd': case 'D': case 77: handleMove(playerRow, playerCol, 0, 1); break;
        }
    }
}

bool jude() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (map[level][i][j] == BOX) {
                return false;
            }
        }
    }
    return true;
}

// 清空键盘缓冲区
void clearKeyBuffer() {
    while (_kbhit()) {
        _getch();
    }
}

void showPassDialog() {
    // 记录关卡得分和步数
    levelSteps[level] = steps;
    levelScores[level] = score;

    cleardevice();

    // 绘制背景
    setfillcolor(RGB(245, 245, 245));
    solidrectangle(0, 0, 400, 400);

    // 绘制卡片
    setfillcolor(WHITE);
    setlinecolor(RGB(220, 220, 220));
    solidrectangle(50, 100, 350, 280);

    // 标题
    settextcolor(RED);
    settextstyle(28, 0, _T("微软雅黑"));
    outtextxy(120, 130, _T("恭喜通关！"));

    // 统计信息
    wchar_t info[50];
    swprintf(info, _countof(info), L"本关步数: %d  得分: %d", steps, score);
    settextcolor(BLACK);
    settextstyle(18, 0, _T("宋体"));
    outtextxy(90, 170, info);

    swprintf(info, _countof(info), L"总步数: %d  总得分: %d", totalSteps, totalScore);
    outtextxy(90, 200, info);

    // 提示文字
    settextcolor(RGB(120, 120, 120));
    settextstyle(16, 0, _T("宋体"));
    outtextxy(120, 230, _T("按任意键进入下一关..."));

    // 刷新绘图
    FlushBatchDraw();

    // 清空键盘缓冲区
    clearKeyBuffer();

    // 等待按键
    while (!_kbhit()) {
        Sleep(50);
    }
    _getch(); // 读取按键
}

// 优化后的通关统计界面
void showAllPassDialog() {
    cleardevice();
    setbkcolor(RGB(245, 245, 245));  // 浅灰色背景

    // 绘制主卡片
    setfillcolor(WHITE);
    setlinecolor(RGB(220, 220, 220));
    solidrectangle(50, 50, 350, 370);

    // 标题区域背景
    setfillcolor(RGB(255, 230, 230));  // 浅红色
    solidrectangle(50, 50, 350, 120);

    // 标题：使用大号字体和喜庆颜色
    settextcolor(RED);
    settextstyle(28, 0, _T("微软雅黑"));
    outtextxy(80, 75, _T("  恭喜完成所有关卡！ "));

    // 统计标题
    settextcolor(RGB(60, 60, 60));
    settextstyle(20, 0, _T("微软雅黑"));
    outtextxy(80, 135, _T("游戏统计详情"));

    // 绘制分隔线
    setlinecolor(RGB(230, 230, 230));
    line(60, 160, 340, 160);

    // 关卡统计
    settextcolor(BLACK);
    settextstyle(18, 0, _T("宋体"));
    for (int i = 0; i < totalLevels; i++) {
        wchar_t info[50];
        swprintf(info, _countof(info), L"● 第 %d 关: %d 步  得分: %d", i + 1, levelSteps[i], levelScores[i]);

        // 每关使用不同的颜色，增强区分度
        COLORREF colors[3] = { RGB(50, 150, 250), RGB(50, 200, 100), RGB(200, 50, 150) };
        settextcolor(colors[i % 3]);

        outtextxy(100, 190 + i * 40, info);
    }

    // 总步数和总得分统计（突出显示）
    settextcolor(RED);
    settextstyle(22, 0, _T("微软雅黑"));
    wchar_t totalInfo[50];
    swprintf(totalInfo, _countof(totalInfo), L"总步数: %d  总得分: %d", totalSteps, totalScore);

    // 绘制总统计背景卡片
    setfillcolor(RGB(255, 240, 240));  // 浅红色背景
    solidrectangle(80, 190 + totalLevels * 40, 320, 230 + totalLevels * 40);

    outtextxy(90, 205 + totalLevels * 40, totalInfo);

    // 绘制底部装饰线
    setlinecolor(RGB(230, 230, 230));
    line(60, 250 + totalLevels * 40, 340, 250 + totalLevels * 40);

    // 提示信息
    settextcolor(RGB(120, 120, 120));
    settextstyle(16, 0, _T("宋体"));
    outtextxy(120, 280 + totalLevels * 40, _T("按任意键关闭游戏..."));

    // 强制刷新绘图
    FlushBatchDraw();

    // 清空键盘缓冲区并等待按键
    clearKeyBuffer();
    while (!_kbhit()) {
        Sleep(50);
    }
    _getch();
}

int main() {
    initGraphics();

    // 初始化初始地图数据
    initInitialMap();

    if (!loadImages()) {
        MessageBox(NULL, L"图片加载失败，游戏可能无法正常显示", L"警告", MB_OK | MB_ICONWARNING);
    }

    int playerRow = playerInitPos[level][0];
    int playerCol = playerInitPos[level][1];

    // 记录进入第一关时的总步数和总得分（初始为0）
    entryTotalSteps = totalSteps;
    entryTotalScore = totalScore;

    map[level][playerRow][playerCol] = PLAYER;
    drawGame();

    while (1) {
        keyevent(&playerRow, &playerCol);

        if (jude()) {
            levelSteps[level] = steps;
            levelScores[level] = score;

            // 对于最后一关，直接调用 showAllPassDialog
            if (level == totalLevels - 1) {
                showAllPassDialog();
                break; // 退出循环
            }
            else {
                showPassDialog(); // 显示当前关卡通关提示

                // 进入下一关时，记录当前总步数和总得分
                entryTotalSteps = totalSteps;
                entryTotalScore = totalScore;

                level++;
                steps = 0;
                score = 0;  // 重置下一关得分

                // 初始化下一关
                playerRow = playerInitPos[level][0];
                playerCol = playerInitPos[level][1];
                map[level][playerRow][playerCol] = PLAYER;
                drawGame();
            }
        }

        Sleep(20);
    }

    closegraph(); // 关闭图形窗口
    return 0;
}