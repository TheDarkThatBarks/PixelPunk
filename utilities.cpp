#include "utilities.hpp"

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void loopFunctions(int n, int startDelay, int delay, void (*startFunc)(), std::vector<void (*)()> funcs) {
    Sleep(startDelay);
    (*startFunc)();
    for (int i = 0; i < n; i++) {
        Sleep(delay);
        (*funcs[0])();
        Sleep(delay);
        for (int j = 1; j < funcs.size(); j++)
            (*funcs[j])();
    }
}

void setCursor(short x, short y) {
    SetConsoleCursorPosition(hConsole, {x, y});
}

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

void setColor(int val) {
    switch (val) {
        case NPC:
            setColor("LIGHT_BLUE", "LIGHT_BLUE");
            break;
        case PLAYER:
            setColor("PURPLE", "PURPLE");
            break;
        case WALL:
            setColor("WHITE", "WHITE");
            break;
        case ENEMY:
            setColor("LIGHT_RED", "LIGHT_RED");
            break;
        default:
            setColor("BLACK", "LIGHT_GRAY");
    }
}

void reset() {
    setCursor(0, 0);
    setColor(0);
}