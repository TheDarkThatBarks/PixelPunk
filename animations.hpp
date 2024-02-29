#pragma once

#include <fstream>
#include <streambuf>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits.h>

#include "utilities.hpp"

#define MENU_SIZE 3

extern const int screenSize;
extern const int rOffset;
extern const int cOffset;

extern const int menuROffset;
extern const int menuCOffset;
extern const std::string menu[MENU_SIZE];
extern std::vector<Pos> menuPos;

extern const int conversationROffset;
extern const int conversationCOffset;
extern const int convoSize;
extern const int maxCharsConvo;
extern std::string currentDialogue;

extern std::string map;

void loadAnimation();
void printScreen();
void printBox();
void printMapBasic();
void clearScreen();
void screenLoad(int width, int height, int rowOffset, int colOffset);
void screenClose(int width, int height, int rowOffset, int colOffset);
void printMenu(int save);

void changeWindow(int width, int time);

void printConvoBox();
//void clearConvoBox();
void conversation(std::string dialogue);
void printConversationText(std::string line);