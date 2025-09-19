#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>  // EasyX ͼ�ο�
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <wchar.h>  // ���ַ�֧��

enum Element { SPACE, WALL, DEST, BOX, PLAYER };

int level = 0, steps = 0, totalSteps = 0, totalLevels = 3;
int score = 0, totalScore = 0;  // ��ǰ�ؿ��÷ֺ��ܵ÷�
int levelSteps[3] = { 0 };
int levelScores[3] = { 0 };     // ��¼ÿ�ص÷�
int entryTotalSteps = 0;        // ���뵱ǰ�ؿ�ʱ���ܲ���
int entryTotalScore = 0;        // ���뵱ǰ�ؿ�ʱ���ܵ÷�
const int CELL_SIZE = 40;

// ��ͼ���ݣ�3 �� �� 10 �� �� 10 �У�
int map[3][10][10] = {
    // �ؿ� 1
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
    // �ؿ� 2
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
    // �ؿ� 3
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

// �����ʼ��ͼ���ݣ��������ã�
int initialMap[3][10][10];
int playerInitPos[3][2] = { {4,4}, {4,4}, {4,4} };

IMAGE img_wall, img_player, img_box, img_dest, img_boxDest, img_playerDest, bg;
bool bgLoaded = false;

// ����ͼƬ
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
    int winHeight = 10 * CELL_SIZE + 100;  // ���Ӹ߶�����ʾ�÷�
    initgraph(winWidth, winHeight);
    setbkcolor(WHITE);
    cleardevice();
}

// ��ʼ����ʼ��ͼ����
void initInitialMap() {
    for (int l = 0; l < totalLevels; l++) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                initialMap[l][i][j] = map[l][i][j];
            }
        }
    }
}

// ���²����͵÷���ʾ
void updateStepDisplay() {
    // ����ײ�����
    setfillcolor(WHITE);
    solidrectangle(0, 10 * CELL_SIZE, 10 * CELL_SIZE, 10 * CELL_SIZE + 100);

    // ��ʾ����
    settextcolor(RED);
    settextstyle(16, 0, _T("����"));
    wchar_t info[50];
    swprintf(info, _countof(info), L"�ؿ�: %d/%d  ����: %d  �ܲ���: %d",
        level + 1, totalLevels, steps, totalSteps);
    outtextxy(10, 10 * CELL_SIZE + 10, info);

    // ��ʾ�÷�
    settextcolor(GREEN);
    swprintf(info, _countof(info), L"��ǰ�÷�: %d  �ܵ÷�: %d", score, totalScore);
    outtextxy(10, 10 * CELL_SIZE + 40, info);

    // ������ʾ
    settextcolor(BLUE);
    settextstyle(14, 0, _T("����"));
    outtextxy(10, 10 * CELL_SIZE + 70, _T("����: W(��) A(��) S(��) D(��) R(����) Q(�˳�)"));
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
    FlushBatchDraw(); // ˢ�»�ͼ
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
    bool boxToDest = false;  // ��¼�����Ƿ��Ƶ�Ŀ�ĵ�

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

        // ��������Ƿ��Ƶ�Ŀ�ĵ�
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

        // ������ӱ��Ƶ�Ŀ�ĵأ����ӵ÷�
        if (boxToDest) {
            score += 10;
            totalScore += 10;
        }

        drawGame(); // �ƶ���ˢ�½��棬���²����͵÷�
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
            // ���õ�ǰ�ؿ�
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    map[level][i][j] = initialMap[level][i][j]; // �ӳ�ʼ��ͼ�ָ�
                }
            }
            *playerRow = playerInitPos[level][0];
            *playerCol = playerInitPos[level][1];
            map[level][*playerRow][*playerCol] = PLAYER; // �ָ����λ��
            steps = 0;
            score = 0;  // ���ùؿ��÷�
            totalSteps = entryTotalSteps;  // �ָ��ܲ���
            totalScore = entryTotalScore;  // �ָ��ܵ÷�
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

// ��ռ��̻�����
void clearKeyBuffer() {
    while (_kbhit()) {
        _getch();
    }
}

void showPassDialog() {
    // ��¼�ؿ��÷ֺͲ���
    levelSteps[level] = steps;
    levelScores[level] = score;

    cleardevice();

    // ���Ʊ���
    setfillcolor(RGB(245, 245, 245));
    solidrectangle(0, 0, 400, 400);

    // ���ƿ�Ƭ
    setfillcolor(WHITE);
    setlinecolor(RGB(220, 220, 220));
    solidrectangle(50, 100, 350, 280);

    // ����
    settextcolor(RED);
    settextstyle(28, 0, _T("΢���ź�"));
    outtextxy(120, 130, _T("��ϲͨ�أ�"));

    // ͳ����Ϣ
    wchar_t info[50];
    swprintf(info, _countof(info), L"���ز���: %d  �÷�: %d", steps, score);
    settextcolor(BLACK);
    settextstyle(18, 0, _T("����"));
    outtextxy(90, 170, info);

    swprintf(info, _countof(info), L"�ܲ���: %d  �ܵ÷�: %d", totalSteps, totalScore);
    outtextxy(90, 200, info);

    // ��ʾ����
    settextcolor(RGB(120, 120, 120));
    settextstyle(16, 0, _T("����"));
    outtextxy(120, 230, _T("�������������һ��..."));

    // ˢ�»�ͼ
    FlushBatchDraw();

    // ��ռ��̻�����
    clearKeyBuffer();

    // �ȴ�����
    while (!_kbhit()) {
        Sleep(50);
    }
    _getch(); // ��ȡ����
}

// �Ż����ͨ��ͳ�ƽ���
void showAllPassDialog() {
    cleardevice();
    setbkcolor(RGB(245, 245, 245));  // ǳ��ɫ����

    // ��������Ƭ
    setfillcolor(WHITE);
    setlinecolor(RGB(220, 220, 220));
    solidrectangle(50, 50, 350, 370);

    // �������򱳾�
    setfillcolor(RGB(255, 230, 230));  // ǳ��ɫ
    solidrectangle(50, 50, 350, 120);

    // ���⣺ʹ�ô�������ϲ����ɫ
    settextcolor(RED);
    settextstyle(28, 0, _T("΢���ź�"));
    outtextxy(80, 75, _T("  ��ϲ������йؿ��� "));

    // ͳ�Ʊ���
    settextcolor(RGB(60, 60, 60));
    settextstyle(20, 0, _T("΢���ź�"));
    outtextxy(80, 135, _T("��Ϸͳ������"));

    // ���Ʒָ���
    setlinecolor(RGB(230, 230, 230));
    line(60, 160, 340, 160);

    // �ؿ�ͳ��
    settextcolor(BLACK);
    settextstyle(18, 0, _T("����"));
    for (int i = 0; i < totalLevels; i++) {
        wchar_t info[50];
        swprintf(info, _countof(info), L"�� �� %d ��: %d ��  �÷�: %d", i + 1, levelSteps[i], levelScores[i]);

        // ÿ��ʹ�ò�ͬ����ɫ����ǿ���ֶ�
        COLORREF colors[3] = { RGB(50, 150, 250), RGB(50, 200, 100), RGB(200, 50, 150) };
        settextcolor(colors[i % 3]);

        outtextxy(100, 190 + i * 40, info);
    }

    // �ܲ������ܵ÷�ͳ�ƣ�ͻ����ʾ��
    settextcolor(RED);
    settextstyle(22, 0, _T("΢���ź�"));
    wchar_t totalInfo[50];
    swprintf(totalInfo, _countof(totalInfo), L"�ܲ���: %d  �ܵ÷�: %d", totalSteps, totalScore);

    // ������ͳ�Ʊ�����Ƭ
    setfillcolor(RGB(255, 240, 240));  // ǳ��ɫ����
    solidrectangle(80, 190 + totalLevels * 40, 320, 230 + totalLevels * 40);

    outtextxy(90, 205 + totalLevels * 40, totalInfo);

    // ���Ƶײ�װ����
    setlinecolor(RGB(230, 230, 230));
    line(60, 250 + totalLevels * 40, 340, 250 + totalLevels * 40);

    // ��ʾ��Ϣ
    settextcolor(RGB(120, 120, 120));
    settextstyle(16, 0, _T("����"));
    outtextxy(120, 280 + totalLevels * 40, _T("��������ر���Ϸ..."));

    // ǿ��ˢ�»�ͼ
    FlushBatchDraw();

    // ��ռ��̻��������ȴ�����
    clearKeyBuffer();
    while (!_kbhit()) {
        Sleep(50);
    }
    _getch();
}

int main() {
    initGraphics();

    // ��ʼ����ʼ��ͼ����
    initInitialMap();

    if (!loadImages()) {
        MessageBox(NULL, L"ͼƬ����ʧ�ܣ���Ϸ�����޷�������ʾ", L"����", MB_OK | MB_ICONWARNING);
    }

    int playerRow = playerInitPos[level][0];
    int playerCol = playerInitPos[level][1];

    // ��¼�����һ��ʱ���ܲ������ܵ÷֣���ʼΪ0��
    entryTotalSteps = totalSteps;
    entryTotalScore = totalScore;

    map[level][playerRow][playerCol] = PLAYER;
    drawGame();

    while (1) {
        keyevent(&playerRow, &playerCol);

        if (jude()) {
            levelSteps[level] = steps;
            levelScores[level] = score;

            // �������һ�أ�ֱ�ӵ��� showAllPassDialog
            if (level == totalLevels - 1) {
                showAllPassDialog();
                break; // �˳�ѭ��
            }
            else {
                showPassDialog(); // ��ʾ��ǰ�ؿ�ͨ����ʾ

                // ������һ��ʱ����¼��ǰ�ܲ������ܵ÷�
                entryTotalSteps = totalSteps;
                entryTotalScore = totalScore;

                level++;
                steps = 0;
                score = 0;  // ������һ�ص÷�

                // ��ʼ����һ��
                playerRow = playerInitPos[level][0];
                playerCol = playerInitPos[level][1];
                map[level][playerRow][playerCol] = PLAYER;
                drawGame();
            }
        }

        Sleep(20);
    }

    closegraph(); // �ر�ͼ�δ���
    return 0;
}