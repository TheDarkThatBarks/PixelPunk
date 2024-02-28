#pragma once

#include <vector>
#include <windows.h>
#include <string>

#define NPC 50
#define PLAYER 1
#define WALL -1
#define ENEMY -5

extern HANDLE hConsole;

void loopFunctions(int n, int startDelay, int delay, void (*startFunc)(), std::vector<void (*)()> funcs);
void setCursor(short x, short y);
void setColor(std::string background, std::string text);
void setColor(int val);
void reset();