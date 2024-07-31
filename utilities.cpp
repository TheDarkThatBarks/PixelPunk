#include "utilities.hpp"

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
HWND console = GetConsoleWindow();
RECT r;

int windowWidth = ORIGINAL_WINDOW_WIDTH;
int windowHeight = ORIGINAL_WINDOW_HEIGHT;

Pos screenPos;
std::vector<std::vector<int>> mapCoord;
std::vector<std::vector<Cell>> frames[2];

int rows, cols;
Pos* playerPos = (Pos*)malloc(sizeof(Pos));

// Converts screen coordinate to map coordinate
Pos screenToMap(Pos pos) {
    return {
        pos.r + screenPos.r,
        pos.c + screenPos.c
    };
}

// Converts map coordinate to screen coordinate
Pos mapToScreen(Pos pos) {
    return {
        pos.r - screenPos.r,
        pos.c - screenPos.c
    };
}

// Runs a start function then loops through the funcs list, running each function with a delay, an n number of times
void loopFunctions(int n, int startDelay, int delay, void (*startFunc)(), std::vector<void (*)()> funcs) {
    Sleep(startDelay);
    (*startFunc)();
    for (int i = 0; i < n; i++) {
        Sleep(delay);
        (*funcs[0])();
        Sleep(delay);
        for (int j = 1; j < (int)funcs.size(); j++)
            (*funcs[j])();
    }
}

// Sets the console cursor position to a specific coordinate
// This function exists to use a more intuitive row by column coordinate system
void setCursor(short r, short c) {
    SetConsoleCursorPosition(hConsole, {c, r});
}

// Sets the console text color based on background and foreground colors
void setColor(std::string background, std::string text) {
    const std::string colors[16] = {"BLACK", "DARK_BLUE", "GREEN", "LIGHT_BLUE", "RED", "PURPLE", "YELLOW", "LIGHT_GRAY", "DARK_GRAY",
                                    "BLUE", "LIGHT_GREEN", "LIGHTEST_BLUE", "LIGHT_RED", "LIGHT_PURPLE", "LIGHT_YELLOW", "WHITE"};
    int bgCode = -1, tCode = -1;
    for (int i = 0; (bgCode < 0 || tCode < 0) && i < 16; i++) {
        if (colors[i] == background)
            bgCode = i;
        if (colors[i] == text)
            tCode = i;
    }
    SetConsoleTextAttribute(hConsole, (bgCode * 16) + tCode);
}

// Sets the console text color based on value of cell, or '0' for default colors (black background, light gray foreground)
void setColor(int val) {
    switch (val) {
        case M_NPC:
            setColor("LIGHT_BLUE", "LIGHT_BLUE");
            break;
        case M_PLAYER:
            setColor("PURPLE", "PURPLE");
            break;
        case M_WALL:
            setColor("WHITE", "WHITE");
            break;
        case M_ENEMY:
            setColor("LIGHT_RED", "LIGHT_RED");
            break;
        default:
            setColor("BLACK", "LIGHT_GRAY");
    }
}

// Sets the console text color based on the type of the given cell
void setColorCell(Cell c) {
    //SetConsoleTextAttribute(hConsole, ((c.isPlayer ? 5 : (c.isEnemy ? 12 : (c.isNPC ? 9 : c.back))) * 16) + (c.isPlayer ? 5 : (c.isEnemy ? 12 : c.fore)));
    SetConsoleTextAttribute(hConsole, ((c.type == '+' ? 5 : (c.type == '-' ? 12 : (c.isNPC ? 9 : c.back))) * 16) + (c.type == '+' ? 5 : c.fore));
}

// Resets the console cursor position and the console text color to default
void reset() {
    setCursor(0, 0);
    setColor(0);
}

// Removes the scrollbar from the window
void removeScrollbar() {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsole, &info);
	COORD newSize = {
		(short)(info.srWindow.Right - info.srWindow.Left + 1),
		(short)(info.srWindow.Bottom - info.srWindow.Top + 1)
	};
	SetConsoleScreenBufferSize(hConsole, newSize);

    CONSOLE_CURSOR_INFO* info2 = (CONSOLE_CURSOR_INFO*)malloc(sizeof(CONSOLE_CURSOR_INFO));
    GetConsoleCursorInfo(hConsole, info2);
    info2->bVisible = false;
    SetConsoleCursorInfo(hConsole, info2);
}