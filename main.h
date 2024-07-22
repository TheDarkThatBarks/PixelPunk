#pragma once

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <io.h>
#include <chrono>

#include "animations.hpp"

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27
#define KB_SPACE 32
#define KB_TAB 9
#define KB_ENTER 13

#define MOVE 0
#define MENU 1

int rows, cols;

int kbMode = MOVE;
int selection = 0;

const int screenThreshold = 3;

Pos* playerPos = (Pos*)malloc(sizeof(Pos));
std::vector<Pos*> enemyPos;

struct Node {
    int r, c, f, g;
    Node* prev;

    bool operator==(Node n) {
        return (n.r == r && n.c == c);
    }
};

void initMap(std::string map);
void initMap2(std::string map);
void updateDisplay(int oldR, int oldC, int newR, int newC);
void changePos(Pos* pos, int newR, int newC, bool player);
void keyPress();

Node* nodeInit(int r, int c, int f, int g, Node* prev);
std::vector<Node*> createPath(Node* current);
int heuristic(Node node);
Node* findMin(std::vector<Node*> list);
std::vector<Node*> pathfind(Node start, Node goal);
void enemyAI();

void updateSelection();
void updateSelection(char dir);