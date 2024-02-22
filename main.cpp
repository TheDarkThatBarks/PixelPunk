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

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27
#define KB_SPACE 32

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
HWND console = GetConsoleWindow();
RECT r;

void removeScrollbar() {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsole, &info);
	COORD new_size = {
		(short)(info.srWindow.Right - info.srWindow.Left + 1),
		(short)(info.srWindow.Bottom - info.srWindow.Top + 1)
	};
	SetConsoleScreenBufferSize(hConsole, new_size);
}

void setCursor(short x, short y) {
    SetConsoleCursorPosition(hConsole, {x, y});
}

void setColor(std::string background, std::string text) {
    int code = 0;
    const std::string colors[16] = {"BLACK", "DARK_BLUE", "GREEN", "LIGHT_BLUE", "RED", "PURPLE", "YELLOW", "LIGHT_GRAY", "DARK_GRAY",
                                    "BLUE", "LIGHT_GREEN", "LIGHTEST_BLUE", "LIGHT_RED", "LIGHT_PURPLE", "LIGHT_YELLOW", "WHITE"};
    int bgCode = 0, tCode = 0;
    for (int i = 0; i < 16; i++) {
        if (colors[i] == background)
            bgCode = i;
        if (colors[i] == text)
            tCode = i;
    }
    code = bgCode * 16 + tCode;
    SetConsoleTextAttribute(hConsole, code);
}

void setColor(char ch) {
    switch (ch) {
        case '+':
            setColor("PURPLE", "PURPLE");
            break;
        default:
            setColor("BLACK", "LIGHT_GRAY");
    }
}

std::vector<std::vector<int>> mapCoord;
int rows, cols;
int rOffset = 3;
int cOffset = 6;
struct {
    int r;
    int c;
} playerPos;

void initMap(std::string map) {
    rows = 1;
    //std::cout << map.length() << std::endl;
	for (int i = 0; i < map.length(); i++) {
        //std::cout << map.at(i);
		if (map.at(i) == '\n')
			rows++;
	}
	cols = map.find('\n') / 2;
    //std::cout << rows << " " << cols << std::endl;
    mapCoord = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));
    std::istringstream f(map);
	std::string line;
    int r = 0;
    while (getline(f, line)) {
        for (int c = 0; c < line.length(); c += 2) {
            char ch = line.at(c);
            int val = 0;
            switch (ch) {
                case '+':
                    val = 1;
                    playerPos.r = r;
                    playerPos.c = c / 2;
                    break;
                case '*':
                    val = -1;
                    break;
                case '-':
                    val = -5;
                    break;
            }
            mapCoord[r][(int)(c / 2)] = val;
        }
        r++;
    }
    setCursor(cOffset, rOffset - 1);
    for (int c = 0; c < cols; c++)
        std::cout << "__";
    for (int r = 0; r < rows; r++) {
        setCursor(cOffset - 1, rOffset + r);
        std::string str = "|";
        for (int c = 0; c < cols; c++)
            str += r == rows - 1 ? "__" : "  ";
        std::cout << str << "|";
    }
}

void printScreen() {
    for (int r = 0; r < rows; r++) {
        setCursor(cOffset, rOffset + r);
        setColor("BLACK", "LIGHT_GRAY");
        for (int c = 0; c < cols; c++) {
            switch (mapCoord[r][c]) {
                case 1:
                    setColor("PURPLE", "PURPLE");
                    break;
                case -1:
                    setColor("WHITE", "WHITE");
                    break;
                case -5:
                    setColor("LIGHT_RED", "LIGHT_RED");
                    break;
                default:
                    setColor("BLACK", "LIGHT_GRAY");
            }
            std::cout << (r == rows - 1 ? "__" : "  ");
        }
        setColor("BLACK", "LIGHT_GRAY");
    }
    setCursor(0, 0);
    setColor("BLACK", "LIGHT_GRAY");
}

void keyPress();

int main() {
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 355, 375, TRUE);
    removeScrollbar();
    std::ifstream m("C:/Users/ellys/source/repos/SquareRPG/map.txt");
	std::string map((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
    initMap(map);
    printScreen();
    keyPress();
}

void updateDisplay(char ch, int oldR, int oldC, int newR, int newC) {
    setCursor(cOffset + oldC * 2, rOffset + oldR);
    setColor(' ');
    std::cout << (oldR == rows - 1 ? "__" : "  ");
    setCursor(cOffset + newC * 2, rOffset + newR);
    setColor(ch);
    std::cout << ch << ch;
    setCursor(0, 0);
    setColor(' ');
}

void changePos(int rChange, int cChange) {
    int newR = playerPos.r + rChange;
    int newC = playerPos.c + cChange;
    if (newR >= 0 && newR < rows && newC >= 0 && newC < cols && mapCoord[newR][newC] >= 0) {
        mapCoord[playerPos.r][playerPos.c] = 0;
        mapCoord[newR][newC] = 1;
        updateDisplay('+', playerPos.r, playerPos.c, newR, newC);
        playerPos.r = newR;
        playerPos.c = newC;
    }
}

void keyPress() {
    int kbCode;
    while (kbCode != KB_ESCAPE) {
        if (kbhit()) {
            kbCode = getch();
            int rChange = 0, cChange = 0;
            switch (kbCode) {
                case KB_UP:
                    rChange--;
                    break;
                case KB_DOWN:
                    rChange++;
                    break;
                case KB_LEFT:
                    cChange--;
                    break;
                case KB_RIGHT:
                    cChange++;
                    break;
            }
            changePos(rChange, cChange);
        }
    }
}