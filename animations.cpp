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
const int convoSize = screenSize * 4;
const int maxCharsConvo = convoSize - 9;
std::string currentDialogue;

void loadAnimation() {
    std::vector<void (*)()> funcs;

    Sleep(500);
    expandWindow(890, 700);
    Sleep(500);
    conversation("Dialogue1.txt");

    Sleep(500);
    screenLoad();

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
}

void printScreen() {
    for (int r = 0; r < screenSize; r++) {
        setCursor(cOffset, rOffset + r);
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
    setCursor(cOffset, rOffset - 1);
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
            setCursor(cOffset, rOffset + screenR);
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

void screenLoad() {
    int middle = std::round((float)screenSize / 2);
    for (int width = 0; width < middle; width++) {
        std::string str(screenSize * 2, '_');
        for (int r = 0; r < width * 2 + 1; r++)
            str += '\n' + std::string(cOffset - 1, ' ') + '|' + std::string(screenSize * 2, r == width * 2 ? '_' : ' ') + '|';
        setCursor(cOffset, rOffset + middle - width - 2);
        printf(str.c_str());
        Sleep(50);
    }
}

void printMenu(int save) {
    const int usableScreen = (cOffset * 2) + (screenSize * 2) - (menuCOffset * 2);
    int gaps[MENU_SIZE];
    gaps[1] = std::round((usableScreen - ((MENU_SIZE + 1) * ((float)(menu[0].length() + menu[1].length()) / 2))) / (MENU_SIZE + 1));
    gaps[0] = std::round(gaps[1] + ((float)menu[1].length() / 2));

    CONSOLE_SCREEN_BUFFER_INFO* info = (CONSOLE_SCREEN_BUFFER_INFO*)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    
    setCursor(menuCOffset, rOffset + screenSize + menuROffset);
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
}

void expandWindow(int width, int time) {
    time *= 0.9;
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style |= WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    float speed = (float)(width - windowWidth) / time;
    int sleepTime = 0, increment = INT_MAX;
    for (int i = 20; i < width - windowWidth; i++) {
        for (int j = 1; (i * j) <= (width - windowWidth); j++) {
            if (((((float)(width - windowWidth) / j) * i) > (int)(time * 0.98)) && ((((float)(width - windowWidth) / j) * i) < (int)(time * 1.02)) && j < increment) {
                sleepTime = i;
                increment = j;
            }
        }
    }
    while (windowWidth <= width) {
        windowWidth += increment;
        SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
        removeScrollbar();
        Sleep(sleepTime);
    }
    windowWidth = width;
    SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
}

void printConvoBox() {
    setCursor(conversationCOffset, conversationROffset - 1);
    printf(std::string(convoSize, '_').c_str());
    for (int r = 0; r < screenSize; r++) {
        setCursor(conversationCOffset - 1, conversationROffset + r);
        printf("|%s|", std::string(convoSize, r == screenSize - 1 ? '_' : ' ').c_str());
    }
    reset();
}

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

    printConvoBox();
    Sleep(500);

    for (int i = 0, r = 0; i < lines.size(); i++) {
        pos = lines[i].find(":");
        int speaker = std::stoi(lines[i].substr(0, pos));
        setCursor(conversationCOffset + 2, conversationROffset + 2 * i + r + 1);
        setColor(speaker);
        printf("  ");
        setColor(0);
        printf(" :");
        std::string str = lines[i].substr(pos + 1);
        std::vector<std::string> strs;
        while (str.length() > maxCharsConvo) {
            strs.push_back(str.substr(0, maxCharsConvo));
            str.erase(0, maxCharsConvo);
        }
        strs.push_back(str);
        for (int j = 0; j < strs.size(); j++) {
            r += (j ? 1 : 0);
            setCursor(conversationCOffset + 7, conversationROffset + 2 * i + r + 1);
            printConversationText(strs[j]);
        }
        Sleep(500);
    }
}

void printConversationText(std::string line) {
    for (char c : line) {
        printf("%c", c);
        Sleep(10);
    }
}