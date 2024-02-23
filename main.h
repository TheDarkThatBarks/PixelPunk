#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <cmath>
#include <map>
#include <queue>
#include <algorithm>
#include <limits.h>

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27
#define KB_SPACE 32

#define PLAYER 1
#define WALL -1
#define ENEMY -5

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
HWND console = GetConsoleWindow();
RECT r;

std::vector<std::vector<int>> mapCoord;
int rows, cols;
const int rOffset = 3;
const int cOffset = 6;

struct Pos {
    int r, c;
};

Pos* playerPos = (Pos*)malloc(sizeof(Pos));
std::vector<Pos*> enemyPos;

struct Node {
    int r, c, f, g;
    Node* prev;

    bool operator==(Node n) {
        return (n.r == r && n.c == c);
    }
};

void removeScrollbar();
void setCursor(short x, short y);
void setColor(std::string background, std::string text);
void setColor(int val);
void reset();

void initMap(std::string map);
void updateDisplay(int val, int oldR, int oldC, int newR, int newC);
void changePos(int val, Pos* pos, int newR, int newC);
void keyPress();

Node* nodeInit(int r, int c, int f, int g, Node* prev);
std::vector<Node*> createPath(Node* current);
int heuristic(Node node);
Node* findMin(std::vector<Node*> list);
std::vector<Node*> pathfind(Node start, Node goal);
void enemyAI();