#include "animations.hpp"

const int screenSize = 15;
const int rOffset = 3;
const int cOffset = 6;

const int menuROffset = 2;
const int menuCOffset = 3;
const std::string menu[MENU_SIZE] = {"TEST1", "TEST2", "TEST3"};
std::vector<Pos> menuPos;

const int conversationROffset = 3;
const int conversationCOffset = cOffset + screenSize * 2 + 7;
const int convoSize = screenSize * 2;
const int maxCharsConvo = convoSize * 2 - 9;
std::string currentDialogue;

void loadAnimation() {
    std::vector<void (*)()> funcs;

    Sleep(500);
    screenLoad(screenSize, screenSize, rOffset, cOffset);

    funcs.push_back(&clearScreen);
    funcs.push_back(&printBox);
    //loopFunctions(3, 500, 50, &printBox, funcs);

    funcs.push_back(&printMapBasic);
    loopFunctions(3, 500, 50, &printMapBasic, funcs);

    funcs.clear();
    funcs.push_back(&printMapBasic);
    funcs.push_back(&printScreen);
    loopFunctions(3, 500, 50, &printScreen, funcs);

    Sleep(500);
    printMenu(1);

    Sleep(500);
    changeWindow(890, 700);
    Sleep(500);
    conversation("Dialogue1.txt");
    Sleep(1000);
    //clearConvoBox();
    screenClose(convoSize, screenSize, conversationROffset, conversationCOffset);
    changeWindow(ORIGINAL_WINDOW_WIDTH, 700);
}

void printScreen() {
    for (int r = 0; r < screenSize; r++) {
        setCursor(rOffset + r, cOffset);
        for (int c = 0; c < screenSize; c++) {
            Pos pos = screenToMap({r, c});
            setColor(mapCoord[pos.r][pos.c]);
            printf(r == screenSize - 1 ? "__" : "  ");
        }
    }
}

void printBox() {
    std::string str(screenSize * 2, '_');
    for (int r = 0; r < screenSize; r++)
        str += '\n' + std::string(cOffset - 1, ' ') + '|' + std::string(screenSize * 2, r == screenSize - 1 ? '_' : ' ') + '|';
    setCursor(rOffset - 1, cOffset);
    printf(str.c_str());
    reset();
}

void printMapBasic() {
    setColor(0);
    std::istringstream f(map);
	std::string line;
    int mapR = 0, screenR = 0;
    while (getline(f, line)) {
        if (mapR >= screenPos.r && mapR < screenPos.r + screenSize) {
            std::string str = line.substr(screenPos.c * 2, screenSize * 2);
            std::replace(str.begin(), str.end(), '0', ' ');
            std::replace(str.begin(), str.end(), '1', '*');
            if (screenR == screenSize - 1)
                std::replace(str.begin(), str.end(), ' ', '_');
            setCursor(rOffset + screenR, cOffset);
            printf(str.c_str());
            screenR++;
        }
        mapR++;
    }
}

void clearScreen() {
    std::string str = "";
    for (int r = 0; r < rOffset + screenSize; r++)
        str += std::string(cOffset + screenSize * 2 + 1, ' ') + '\n';
    reset();
    printf(str.c_str());
}

void screenLoad(int width, int height, int rowOffset, int colOffset) {
    int middle = std::round((float)height / 2);
    for (int h = 0; h < middle; h++) {
        setCursor(rowOffset + middle - 2 - h, colOffset);
        printf(std::string(width * 2, '_').c_str());
        for (int r = middle - h - 1; r < middle + h; r++) {
            setCursor(rowOffset + r, colOffset - 1);
            printf("|%s|", std::string(width * 2, r == middle + h - 1 ? '_' : ' ').c_str());
        }
        Sleep(50);
    }
}

void screenClose(int width, int height, int rowOffset, int colOffset) {
    int middle = std::round((float)height / 2);
    for (int h = middle - 2; h >= 0; h--) {
        setCursor(rowOffset + middle - 3 - h, colOffset - 1);
        printf(std::string(width * 2 + 2, ' ').c_str());
        setCursor(rowOffset + middle - 2 - h, colOffset - 1);
        printf(" %s ", std::string(width * 2, '_').c_str());
        setCursor(rowOffset + middle + h - 1, colOffset - 1);
        printf("|%s|", std::string(width * 2, '_').c_str());
        setCursor(rowOffset + middle + h, colOffset - 1);
        printf(std::string(width * 2 + 2, ' ').c_str());
        Sleep(50);
    }
    setCursor(rowOffset + middle - 2, colOffset - 1);
    printf(std::string(width * 2 + 2, ' ').c_str());
    setCursor(rowOffset + middle - 1, colOffset - 1);
    printf(std::string(width * 2 + 2, ' ').c_str());
}

void printMenu(int save) {
    const int usableScreen = (cOffset * 2) + (screenSize * 2) - (menuCOffset * 2);
    int gaps[MENU_SIZE];
    gaps[1] = std::round((usableScreen - ((MENU_SIZE + 1) * ((float)(menu[0].length() + menu[1].length()) / 2))) / (MENU_SIZE + 1));
    gaps[0] = std::round(gaps[1] + ((float)menu[1].length() / 2));

    CONSOLE_SCREEN_BUFFER_INFO* info = (CONSOLE_SCREEN_BUFFER_INFO*)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    
    setCursor(rOffset + screenSize + menuROffset, menuCOffset);
    setColor(0);
    for (int i = 0; i < MENU_SIZE; i++) {
        if (i > 1)
            gaps[i] = std::round(gaps[i - 1] + ((float)(menu[i - 2].length() - menu[i].length()) / 2));
        if (save) {
            GetConsoleScreenBufferInfo(hConsole, info);
            menuPos.push_back({info->dwCursorPosition.Y, info->dwCursorPosition.X + gaps[i]});
        }
        printf("%s%s", std::string(gaps[i], ' ').c_str(), menu[i].c_str());
    }
    reset();
}

void changeWindow(int width, int time) {
    time *= 0.9;
    int sleepTime = 0, increment = INT_MAX;
    for (int i = 20; i < std::abs(width - windowWidth); i++) {
        for (int j = 1; j < increment && i * j <= std::abs(width - windowWidth); j++) {
            if (((float)std::abs(width - windowWidth) / j) * i > time * 0.98 && ((float)std::abs(width - windowWidth) / j) * i < time * 1.02) {
                sleepTime = i;
                increment = j;
            }
        }
    }
    if (sleepTime == 0 && increment == INT_MAX)
        return;
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style |= WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    if (width > windowWidth) {
        while (windowWidth <= width) {
            windowWidth += increment;
            SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
            removeScrollbar();
            Sleep(sleepTime);
        }
    } else {
        while (windowWidth >= width) {
            windowWidth -= increment;
            SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
            removeScrollbar();
            Sleep(sleepTime);
        }
    }
    windowWidth = width;
    SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
}

void printConvoBox() {
    setCursor(conversationROffset - 1, conversationCOffset);
    printf(std::string(convoSize, '_').c_str());
    for (int r = 0; r < screenSize; r++) {
        setCursor(conversationROffset + r, conversationCOffset - 1);
        printf("|%s|", std::string(convoSize, r == screenSize - 1 ? '_' : ' ').c_str());
    }
    reset();
}

/*void clearConvoBox() {
    for (int r = -1; r < screenSize; r++) {
        setCursor(r + conversationROffset, conversationCOffset - 1);
        printf(std::string(convoSize * 2 + 2, ' ').c_str());
    }
    reset();
}*/

void conversation(std::string dialogue) {
    std::ifstream d(dialogue);
	currentDialogue = std::string((std::istreambuf_iterator<char>(d)), std::istreambuf_iterator<char>());
    std::vector<std::string> lines;
    size_t pos = 0;
    while ((pos = currentDialogue.find('\n')) != std::string::npos) {
        std::string str = currentDialogue.substr(0, pos);
        lines.push_back(str);
        currentDialogue.erase(0, pos + 1);
    }
    lines.push_back(currentDialogue);

    screenLoad(convoSize, screenSize, conversationROffset, conversationCOffset);
    Sleep(500);

    for (int i = 0, r = 0; i < lines.size(); i++) {
        pos = lines[i].find(":");
        int speaker = std::stoi(lines[i].substr(0, pos));
        setCursor(conversationROffset + 2 * i + r + 1, conversationCOffset + 2);
        setColor(speaker);
        printf("  ");
        setColor(0);
        printf(" :");
        Sleep(500);
        std::string str = lines[i].substr(pos + 1);
        std::vector<std::string> strs;
        while (str.length() > maxCharsConvo) {
            strs.push_back(str.substr(0, maxCharsConvo));
            str.erase(0, maxCharsConvo);
        }
        strs.push_back(str);
        for (int j = 0; j < strs.size(); j++) {
            r += (j ? 1 : 0);
            setCursor(conversationROffset + 2 * i + r + 1, conversationCOffset + 7);
            printConversationText(strs[j]);
        }
        Sleep(500);
    }
}

void printConversationText(std::string line) {
    for (char c : line) {
        printf("%c", c);
        Sleep(20);
    }
}