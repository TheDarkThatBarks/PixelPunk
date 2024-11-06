#pragma once

#include <vector>
#include <windows.h>
#include <string>
#include <chrono>

#define M_NPC 50
#define M_PLAYER 1
#define M_WALL -1
#define M_ENEMY -5
#define M_TEXT 100

#define ORIGINAL_WINDOW_WIDTH 355
#define ORIGINAL_WINDOW_HEIGHT 408
#define TITLE_WINDOW_WIDTH 539
#define TITLE_WINDOW_HEIGHT 300
#define CONVERSATION_WINDOW_WIDTH 890

#define FRAMES 2

extern HANDLE hConsole;
extern HWND console;
extern RECT r;

extern int windowWidth;
extern int windowHeight;

extern int rows, cols;

struct Pos {
    int r, c;
    Pos(int R, int C) : r(R), c(C) {};

    bool operator==(Pos p) {
        return r == p.r && c == p.c;
    }

    /*bool operator==(Pos* p) {
        return r == p->r && c == p->c;
    }*/
};
extern Pos* playerPos;

struct Cell {
    char type;
    char value;
    int fore;
    int back;
    bool isPlayer;
    bool isEnemy;
    bool isNPC;
    bool isStart;

    /*bool isEnemy() {
        return type == '-';
    }*/

    bool operator==(Cell c) {
        return c.value == value && c.fore == fore && c.back == back /*&& c.value == value && c.fore == fore && c.back == back && c.isPlayer == isPlayer && c.isEnemy == isEnemy*/;
    }
};

struct Projectile {
    Pos coord;
    char ch;
};

extern std::vector<std::vector<Cell>> frames[FRAMES];
extern std::vector<std::vector<int>> mapCoord;
extern std::vector<std::string> mapText;
extern int textIndex;

extern Pos screenPos;
Pos screenToMap(Pos pos);
Pos mapToScreen(Pos pos);

void loopFunctions(int n, int startDelay, int delay, void (*startFunc)(), std::vector<void (*)()> funcs);
void setCursor(short r, short c);
void setColor(std::string background, std::string text);
void setColor(int val);
void setColorCell(Cell c);
int computeColor(Cell c);
void reset();
void removeScrollbar();