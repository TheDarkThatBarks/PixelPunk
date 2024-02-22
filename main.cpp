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
    if (background == "DARK_BLUE") {
        code += 16*1;
    } else if (background == "GREEN") {
        code += 16*2;
    } else if (background == "LIGHT_BLUE") {
        code += 16*3;
    } else if (background == "RED") {
        code += 16*4;
    } else if (background == "PURPLE") {
        code += 16*5;
    } else if (background == "YELLOW") {
        code += 16*6;
    } else if (background == "LIGHT_GRAY") {
        code += 16*7;
    } else if (background == "DARK_GRAY") {
        code += 16*8;
    } else if (background == "BLUE") {
        code += 16*9;
    } else if (background == "LIGHT_GREEN") {
        code += 16*10;
    } else if (background == "LIGHTEST_BLUE") {
        code += 16*11;
    } else if (background == "LIGHT_RED") {
        code += 16*12;
    } else if (background == "LIGHT_PURPLE") {
        code += 16*13;
    } else if (background == "LIGHT_YELLOW") {
        code += 16*14;
    } else if (background == "WHITE") {
        code += 16*15;
    }
    if (text == "DARK_BLUE") {
        code += 1;
    } else if (text == "GREEN") {
        code += 2;
    } else if (text == "LIGHT_BLUE") {
        code += 3;
    } else if (text == "RED") {
        code += 4;
    } else if (text == "PURPLE") {
        code += 5;
    } else if (text == "YELLOW") {
        code += 6;
    } else if (text == "LIGHT_GRAY") {
        code += 7;
    } else if (text == "DARK_GRAY") {
        code += 8;
    } else if (text == "BLUE") {
        code += 9;
    } else if (text == "LIGHT_GREEN") {
        code += 10;
    } else if (text == "LIGHTEST_BLUE") {
        code += 11;
    } else if (text == "LIGHT_RED") {
        code += 12;
    } else if (text == "LIGHT_PURPLE") {
        code += 13;
    } else if (text == "LIGHT_YELLOW") {
        code += 14;
    } else if (text == "WHITE") {
        code += 15;
    }
    SetConsoleTextAttribute(hConsole, code);
}

std::vector<std::vector<int>> mapCoord;
int rows, cols;

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
}

void printScreen() {
    int rOffset = 3;
    int cOffset = 5;
    for (int r = -1; r < rows; r++) {
        setCursor(cOffset, rOffset + r);
        setColor("BLACK", "LIGHT_GRAY");
        if (r == -1) {
            std::cout << " ";
        } else {
            std::cout << "|";
        }
        for (int c = 0; c < cols; c++) {
            if (r == -1) {
                std::cout << "__";
            } else {
                char ch = ' ';
                switch (mapCoord[r][c]) {
                    case 1:
                        ch = '+';
                        setColor("PURPLE", "PURPLE");
                        break;
                    case -1:
                        ch = '*';
                        setColor("WHITE", "WHITE");
                        break;
                    case -5:
                        ch = '-';
                        setColor("LIGHT_RED", "LIGHT_RED");
                        break;
                    default:
                        if (r == rows - 1)
                            ch = '_';
                        setColor("BLACK", "LIGHT_GRAY");
                }
                std::cout << ch << ch;
            }
        }
        setColor("BLACK", "LIGHT_GRAY");
        if (r == -1) {
            std::cout << " ";
        } else {
            std::cout << "|";
        }
        //std::cout << std::endl;
    }
    //setCursor(0, 0);
    setColor("BLACK", "LIGHT_GRAY");
}

int main() {
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 355, 375, TRUE);
    removeScrollbar();
    std::ifstream m("C:/Users/ellys/source/repos/SquareRPG/map.txt");
	std::string map((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
    initMap(map);
    printScreen();
}