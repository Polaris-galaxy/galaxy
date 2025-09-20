#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
//gcc 3.c -o 3.exe -luser32 -lgdi32
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
char levelNames[3][20] = {"基础训练", "十字迷宫", "星形挑战"}; // 关卡名称
int cellSize = 40;       // 格子大小（像素）
int playerRow, playerCol; // 玩家位置
int windowWidth = 800;   // 窗口宽度
int windowHeight = 600;  // 窗口高度

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

// 状态存储结构（用于撤销操作）
typedef struct {
    int map[10][10];
    int playerRow;
    int playerCol;
    int steps;
} GameState;

GameState lastState;  // 上一步状态
bool canUndo = false; // 是否可以撤销

// 位图资源
HBITMAP hWallBmp = NULL;
HBITMAP hDestBmp = NULL;
HBITMAP hBoxBmp = NULL;
HBITMAP hPlayerBmp = NULL;
HBITMAP hPlayerOnDestBmp = NULL;
HBITMAP hBoxOnDestBmp = NULL;
HBITMAP hSpaceBmp = NULL;

// 加载位图资源
void LoadBitmaps() {
    // 创建简单的颜色位图（实际应用中可以使用真实图片）
    HDC hdc = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdc);
    
    // 创建不同颜色的位图代表不同元素
    hWallBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hWallBmp);
    HBRUSH wallBrush = CreateSolidBrush(RGB(100, 100, 100));
    RECT wallRect = {0, 0, cellSize, cellSize};
    FillRect(hdcMem, &wallRect, wallBrush);
    DeleteObject(wallBrush);
    
    hDestBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hDestBmp);
    HBRUSH destBrush = CreateSolidBrush(RGB(255, 200, 200));
    FillRect(hdcMem, &wallRect, destBrush);
    DeleteObject(destBrush);
    
    hBoxBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hBoxBmp);
    HBRUSH boxBrush = CreateSolidBrush(RGB(200, 150, 100));
    FillRect(hdcMem, &wallRect, boxBrush);
    DeleteObject(boxBrush);
    
    hPlayerBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hPlayerBmp);
    HBRUSH playerBrush = CreateSolidBrush(RGB(100, 100, 255));
    FillRect(hdcMem, &wallRect, playerBrush);
    DeleteObject(playerBrush);
    
    hPlayerOnDestBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hPlayerOnDestBmp);
    HBRUSH playerOnDestBrush = CreateSolidBrush(RGB(150, 150, 255));
    FillRect(hdcMem, &wallRect, playerOnDestBrush);
    DeleteObject(playerOnDestBrush);
    
    hBoxOnDestBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hBoxOnDestBmp);
    HBRUSH boxOnDestBrush = CreateSolidBrush(RGB(200, 200, 100));
    FillRect(hdcMem, &wallRect, boxOnDestBrush);
    DeleteObject(boxOnDestBrush);
    
    hSpaceBmp = CreateCompatibleBitmap(hdc, cellSize, cellSize);
    SelectObject(hdcMem, hSpaceBmp);
    HBRUSH spaceBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdcMem, &wallRect, spaceBrush);
    DeleteObject(spaceBrush);
    
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);
}

// 释放位图资源
void FreeBitmaps() {
    if (hWallBmp) DeleteObject(hWallBmp);
    if (hDestBmp) DeleteObject(hDestBmp);
    if (hBoxBmp) DeleteObject(hBoxBmp);
    if (hPlayerBmp) DeleteObject(hPlayerBmp);
    if (hPlayerOnDestBmp) DeleteObject(hPlayerOnDestBmp);
    if (hBoxOnDestBmp) DeleteObject(hBoxOnDestBmp);
    if (hSpaceBmp) DeleteObject(hSpaceBmp);
}

// 绘制游戏地图
void DrawMap(HDC hdc) {
    // 绘制背景
    RECT bgRect = {0, 0, windowWidth, windowHeight};
    HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);
    
    // 绘制标题
    HFONT hFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, 
                            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
                            VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(70, 70, 200));
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, 20, 10, TEXT("推箱子游戏"), 10);
    DeleteObject(hFont);
    
    // 绘制状态信息
    hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                      DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, 
                      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
                      VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    char status[256];
    sprintf(status, "关卡: %d/%d [%s]    当前步数: %d    总步数: %d", 
            level + 1, totalLevels, levelNames[level], steps, totalSteps);
    TextOutA(hdc, 20, 50, status, strlen(status));
    
    // 绘制操作说明
    TextOut(hdc, 20, windowHeight - 70, 
            TEXT("操作: ↑←↓→ 移动 | R 重置 | Z 撤销 | B 上一关 | N 下一关"), 50);
    TextOut(hdc, 20, windowHeight - 40, 
            TEXT("M 菜单 | Q 退出"), 20);
    
    DeleteObject(hFont);
    
    // 计算地图绘制起始位置（居中）
    int mapWidth = 10 * cellSize;
    int mapHeight = 10 * cellSize;
    int startX = (windowWidth - mapWidth) / 2;
    int startY = 100;
    
    // 绘制地图格子
    HDC hdcMem = CreateCompatibleDC(hdc);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int x = startX + j * cellSize;
            int y = startY + i * cellSize;
            
            HBITMAP hBmp = NULL;
            switch (map[level][i][j]) {
                case SPACE:     hBmp = hSpaceBmp; break;
                case WALL:      hBmp = hWallBmp; break;
                case DEST:      hBmp = hDestBmp; break;
                case BOX:       hBmp = hBoxBmp; break;
                case PLAYER:    hBmp = hPlayerBmp; break;
                case PLAYER + DEST: hBmp = hPlayerOnDestBmp; break;
                case BOX + DEST:  hBmp = hBoxOnDestBmp; break;
            }
            
            if (hBmp) {
                SelectObject(hdcMem, hBmp);
                BitBlt(hdc, x, y, cellSize, cellSize, hdcMem, 0, 0, SRCCOPY);
            }
            
            // 绘制格子边框
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, x, y, x + cellSize, y + cellSize);
            DeleteObject(hPen);
        }
    }
    DeleteDC(hdcMem);
}

// 保存当前状态（用于撤销）
void saveState() {
    memcpy(lastState.map, map[level], sizeof(lastState.map));
    lastState.playerRow = playerRow;
    lastState.playerCol = playerCol;
    lastState.steps = steps;
    canUndo = true;
}

// 撤销操作
void undoMove(HWND hwnd) {
    if (!canUndo) return;
    
    memcpy(map[level], lastState.map, sizeof(lastState.map));
    playerRow = lastState.playerRow;
    playerCol = lastState.playerCol;
    steps = lastState.steps;
    totalSteps = totalSteps - (steps - lastState.steps);
    
    InvalidateRect(hwnd, NULL, TRUE);
    canUndo = false;
}

// 移动逻辑
bool handleMove(int newRow, int newCol, int boxRow, int boxCol, HWND hwnd) {
    if (newRow < 0 || newRow >= 10 || newCol < 0 || newCol >= 10) {
        return false;
    }

    int target = map[level][newRow][newCol];
    bool moved = false;

    // 保存当前状态（用于撤销）
    saveState();

    // 情况 1：移动玩家（空地/目的地）
    if (target == SPACE || target == DEST) {
        map[level][playerRow][playerCol] -= PLAYER;
        map[level][newRow][newCol] += PLAYER;
        playerRow = newRow;
        playerCol = newCol;
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

        // 移动玩家
        map[level][playerRow][playerCol] -= PLAYER;
        map[level][newRow][newCol] += PLAYER;
        playerRow = newRow;
        playerCol = newCol;
        moved = true;
    }

    if (moved) {
        steps++;
        totalSteps++;
        InvalidateRect(hwnd, NULL, TRUE);
    } else {
        canUndo = false; // 移动失败，撤销无效
    }
    return moved;
}

// 重置当前关卡
void resetLevel() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            map[level][i][j] = map[level][i][j] % 4;
        }
    }
    playerRow = playerInitPos[level][0];
    playerCol = playerInitPos[level][1];
    map[level][playerRow][playerCol] = PLAYER;
    totalSteps = totalSteps - steps + levelSteps[level];
    steps = 0;
    levelSteps[level] = 0;
    canUndo = false;
}

// 判断是否通关
bool isLevelComplete() {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            if (map[level][i][j] == BOX)
                return false;
    return true;
}

// 显示关卡选择菜单
void showLevelMenu(HWND hwnd) {
    const char* items[] = {"基础训练", "十字迷宫", "星形挑战"};
    
    int choice = MessageBoxA(hwnd, "选择关卡:", "关卡选择", 
                           MB_ICONQUESTION | MB_YESNOCANCEL);
    
    if (choice == IDYES && level > 0) {
        level--;
    } 
    else if (choice == IDNO && level < totalLevels - 1) {
        level++;
    }
    else if (choice == IDCANCEL) {
        return;
    }
    
    resetLevel();
    InvalidateRect(hwnd, NULL, TRUE);
}

// 显示通关提示
void showVictoryMessage(HWND hwnd) {
    char message[256];
    sprintf(message, "恭喜通过第 %d 关 [%s]!\n\n本关步数: %d 步\n总步数: %d 步", 
            level + 1, levelNames[level], steps, totalSteps);
    
    int response = MessageBoxA(hwnd, message, "关卡完成", 
                              MB_ICONINFORMATION | MB_OKCANCEL);
    
    if (response == IDOK) {
        if (level < totalLevels - 1) {
            level++;
            resetLevel();
            InvalidateRect(hwnd, NULL, TRUE);
        } else {
            // 所有关卡完成
            char finalMessage[512];
            sprintf(finalMessage, "恭喜完成所有关卡!\n\n各关卡步数统计:\n");
            for (int i = 0; i < totalLevels; i++) {
                char levelMsg[100];
                sprintf(levelMsg, "第 %d 关 [%s]: %d 步\n", i + 1, levelNames[i], levelSteps[i]);
                strcat(finalMessage, levelMsg);
            }
            strcat(finalMessage, "\n总步数: ");
            char totalStepsStr[20];
            sprintf(totalStepsStr, "%d 步", totalSteps);
            strcat(finalMessage, totalStepsStr);
            
            MessageBoxA(hwnd, finalMessage, "游戏通关", MB_ICONINFORMATION | MB_OK);
            PostQuitMessage(0);
        }
    } else {
        // 用户选择取消，重新开始本关
        resetLevel();
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            LoadBitmaps();
            playerRow = playerInitPos[level][0];
            playerCol = playerInitPos[level][1];
            map[level][playerRow][playerCol] = PLAYER;
            break;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawMap(hdc);
            EndPaint(hwnd, &ps);
            break;
        }
            
        case WM_KEYDOWN:
            switch (wParam) {
                case 'Q': 
                    PostQuitMessage(0);
                    break;
                    
                case 'R': // 重置关卡
                    resetLevel();
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                    
                case 'Z': // 撤销操作
                    undoMove(hwnd);
                    break;
                    
                case 'M': // 显示菜单
                    showLevelMenu(hwnd);
                    break;
                    
                case 'B': // 上一关
                    if (level > 0) {
                        level--;
                        resetLevel();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                case 'N': // 下一关
                    if (level < totalLevels - 1) {
                        level++;
                        resetLevel();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                // 方向键处理
                case VK_UP:    // 上
                    handleMove(playerRow-1, playerCol, playerRow-2, playerCol, hwnd);
                    break;
                    
                case VK_DOWN:  // 下
                    handleMove(playerRow+1, playerCol, playerRow+2, playerCol, hwnd);
                    break;
                    
                case VK_LEFT:  // 左
                    handleMove(playerRow, playerCol-1, playerRow, playerCol-2, hwnd);
                    break;
                    
                case VK_RIGHT: // 右
                    handleMove(playerRow, playerCol+1, playerRow, playerCol+2, hwnd);
                    break;
            }
            
            // 检查是否通关
            if (isLevelComplete()) {
                levelSteps[level] = steps;
                showVictoryMessage(hwnd);
            }
            break;
            
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
            
        case WM_DESTROY:
            FreeBitmaps();
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 注册窗口类
BOOL RegisterWindowClass(HINSTANCE hInstance) {
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT("SokobanWindowClass");
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassEx(&wc);
}

// 程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    // 注册窗口类
    if (!RegisterWindowClass(hInstance)) {
        MessageBox(NULL, TEXT("窗口注册失败!"), TEXT("错误"), MB_ICONERROR);
        return 0;
    }
    
    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0,
        TEXT("SokobanWindowClass"),
        TEXT("推箱子游戏"),
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) {
        MessageBox(NULL, TEXT("窗口创建失败!"), TEXT("错误"), MB_ICONERROR);
        return 0;
    }
    
    // 显示窗口
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}