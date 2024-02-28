#pragma once

#include <vector>
#include <windows.h>
#include <string>

#define NPC 50
#define PLAYER 1
#define WALL -1
#define ENEMY -5

extern HANDLE hConsole;
extern HWND console;
extern RECT r;

extern int windowWidth;
extern int windowHeight;

struct Pos {
    int r, c;

    bool operator==(Pos p) {
        return (r == p.r && c == p.c);
    }
};

extern std::vector<std::vector<int>> mapCoord;

extern Pos screenPos;
Pos screenToMap(Pos pos);
Pos mapToScreen(Pos pos);

void loopFunctions(int n, int startDelay, int delay, void (*startFunc)(), std::vector<void (*)()> funcs);
void setCursor(short x, short y);
void setColor(std::string background, std::string text);
void setColor(int val);
void reset();
void removeScrollbar();