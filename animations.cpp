#include "animations.hpp"
#include "json.hpp"
using json = nlohmann::json;

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

int currFrame = 0;
std::vector<Pos> animChangeList;

std::vector<Pos> redrawList;
std::vector<Projectile> projectileList;
Reprint reprint;
std::vector<npcID> npcIDs;
std::vector<EnemyPos*> enemyPos;

std::string title[] = {
    "_______ __               __ _______             __    ",
    "\\____  \\__|__  ___ ____ |  |\\____  \\__ __  ___ |  | __",
    " |   __/  \\  \\/  // __ \\|  | |   __/  |  \\/   \\|  |/ /",
    " |  |  |  |>    <\\  ___/|  |_|  |  |  |  /  |  \\    < ",
    " |__|  |__/__/\\_  \\___  >____/__|  |____/|__|  /__|_ \\",
    "                \\/    \\/                     \\/     \\/"
};

void printTitle() {
    int r = 5;
    for (std::string line : title) {
        setCursor(r, 5);
        printf("%s", line);
        r++;
    }
}

void clearTitle() {
    for (int i = 0; i < 6; i++) {
        setCursor(i + 5, 5);
        for (int j = 0; j < 60; j++)
            printf(" ");
    }
}

void closeTitle() {
    const int rowOffset = 5;
    const int colOffset = 5;
    const int height = 5;
    const int width = 60;
    int middle = std::round((float)height / 2);
    for (int h = middle - 2; h >= 0; h--) {
        setCursor(rowOffset + middle - 2 - h, colOffset);
        printf(std::string(width, ' ').c_str());
        setCursor(rowOffset + middle + h + 1, colOffset);
        printf(std::string(width, ' ').c_str());
        Sleep(50);
        //getch();
    }
    setCursor(rowOffset + middle - 1, colOffset - 1);
    printf(std::string(width, ' ').c_str());
    setCursor(rowOffset + middle, colOffset - 1);
    printf(std::string(width, ' ').c_str());
}

// Runs start animation
void loadAnimation() {
    std::vector<void (*)()> funcs;

    funcs.push_back(&clearTitle);
    funcs.push_back(&printTitle);
    loopFunctions(3, 500, 50, &printTitle, funcs);

    Sleep(2000);
    closeTitle();

    Sleep(500);
    changeWindow(ORIGINAL_WINDOW_WIDTH, ORIGINAL_WINDOW_HEIGHT, 400);

    Sleep(500);
    screenLoad(screenSize, screenSize, rOffset, cOffset);

    funcs.clear();
    funcs.push_back(&clearScreen);
    funcs.push_back(&printBox);
    //loopFunctions(3, 500, 50, &printBox, funcs);

    funcs.push_back(&printMapBasic);
    loopFunctions(3, 500, 50, &printMapBasic, funcs);
    reset();
    //printf("Finished printMapBasic");

    funcs.clear();
    funcs.push_back(&printMapBasic);
    funcs.push_back(&printScreen);
    loopFunctions(3, 500, 50, &printScreen, funcs);

    Sleep(500);
    printMenu(1);

    Sleep(500);
    /*changeWindow(890, 700);
    Sleep(500);
    conversation("Dialogue1.txt");
    Sleep(1000);
    //clearConvoBox();
    screenClose(convoSize, screenSize, conversationROffset, conversationCOffset);
    changeWindow(ORIGINAL_WINDOW_WIDTH, 700);*/
}

// Displays the section of the map contained within the screen bounds
void printScreen() {
    for (int r = 0; r < screenSize; r++) {
        for (int c = 0; c < screenSize * 2; c++)
            printCell(screenToMap({r, c}), {r, c});
    }
}

char computePrintVal(Pos pos, Pos coord) {
    char val = frames[currFrame][pos.r][pos.c].value;
    if (frames[currFrame][pos.r][pos.c / 2 * 2].type == '-') {
        for (EnemyPos* enemy : enemyPos) {
            if (Pos(pos.r, pos.c / 2 * 2) == *(enemy->pos)) {
                val = enemy->type;
                break;
            }
        }
    }
    return coord.r == screenSize - 1 && val == ' ' ? '_' : val;
}

// Prints the cell at a position on the map at a coordinate on the screen
void printCell(Pos pos, Pos coord) {
    /*WORD color = computeColor(frames[currFrame][pos.r][pos.c]);
    char buffer[] = {computePrintVal(pos, coord)};
    DWORD written;
    WriteConsoleOutputAttribute(hConsole, &color, 1, {(short)(cOffset + coord.c), (short)(rOffset + coord.r)}, &written);
    WriteConsoleOutputCharacterA(hConsole, buffer, 1, {(short)(cOffset + coord.c), (short)(rOffset + coord.r)}, &written);*/
    /*CHAR_INFO buffer[1];
    CHAR_INFO b = {
        (CHAR)computePrintVal(pos, coord),
        (WORD)computeColor(frames[currFrame][pos.r][pos.c])
    };
    buffer[0] = b;
    SMALL_RECT s = {
        (SHORT)(cOffset + coord.c),
        (SHORT)(rOffset + coord.r),
        (SHORT)(cOffset + coord.c),
        (SHORT)(rOffset + coord.r)
    };
    WriteConsoleOutput(hConsole, buffer, {1, 1}, {(SHORT)(cOffset + coord.c), (SHORT)(rOffset + coord.r)}, &s);*/
    setCursor(rOffset + coord.r, cOffset + coord.c);
    setColorCell(frames[currFrame][pos.r][pos.c]);
    printf("%c", computePrintVal(pos, coord));
}

// Updates the screen after a player moves
// Goes through reprint list and removes old positions
// If the screen does not need to be shifted, the new positions are drawn
// Otherwise, the screen is reprinted cell by cell
// If the movement of the screen does not change a coordinate's appearance on the screen, it is not reprinted
void updateScreen(int dir) {
    for (int i = 0; i < (int)redrawList.size(); i += reprint.reprint ? 2 : 1) {
        Pos pos = redrawList[i];
        if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
            Pos screenCoord = mapToScreen(pos);
            for (int i = 0; i < 2; i++)
                printCell({pos.r, pos.c + i}, {screenCoord.r, screenCoord.c + i});
        }
    }
    redrawList.clear();
    if (reprint.reprint) {
        //reset();
        //printf("Reprint");
        screenPos.r += reprint.rChange;
        screenPos.c += reprint.cChange;
        for (int r = 0; r < screenSize; r++) {
            for (int c = 0; c < screenSize * 2; c++) {
                Pos pos = screenToMap({r, c});
                if (dir == 0 && pos.r + 1 < rows && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r + 1][pos.c]) {
                    continue;
                } else if (dir == 1 && pos.r > 0 && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r - 1][pos.c]) {
                    continue;
                } else if (dir == 2 && pos.c + 2 < cols && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r][pos.c + 2]) {
                    continue;
                } else if (dir == 3 && pos.c >= 2 && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r][pos.c - 2]) {
                    continue;
                } else {
                    printCell(pos, {r, c});
                }
            }
        }
        reprint.reprint = false;
    }

    for (Projectile projectile : projectileList) {
        setCursor(rOffset + projectile.coord.r, cOffset + projectile.coord.c);
        setColor(0);
        printf("%c", projectile.ch);
        //Sleep(10);
    }
    if (!projectileList.empty())
        Sleep(250);
    for (Projectile projectile : projectileList)
        printCell(screenToMap(projectile.coord), projectile.coord);
    projectileList.clear();
}

// Prints the box outline of the screen
void printBox() {
    std::string str(screenSize * 2, '_');
    for (int r = 0; r < screenSize; r++)
        str += '\n' + std::string(cOffset - 1, ' ') + '|' + std::string(screenSize * 2, r == screenSize - 1 ? '_' : ' ') + '|';
    setCursor(rOffset - 1, cOffset);
    printf(str.c_str());
    reset();
}

// Prints a basic version of the map, types instead of values
void printMapBasic() {
    setColor(0);
    //reset();
    for (int r = 0; r < screenSize; r++) {
        setCursor(rOffset + r, cOffset);
        for (int c = 0; c < screenSize * 2; c++) {
            Pos pos = screenToMap({r, c});
            //printf("%d, %d, %d, %d, ", r, c, screenPos.r, screenPos.c);
            //char val = frames[currFrame][pos.r][pos.c].value;
            //printf("%d, %d, %d, ", currFrame, pos.r, pos.c);
            //printf("%c\n", frames[currFrame][pos.r][pos.c].type);
            printf("%c", frames[currFrame][pos.r][pos.c].type == ' ' ? (r == screenSize - 1 ? '_' : ' ') : frames[currFrame][pos.r][pos.c].type);
        }
    }
}

// Clears the screen by printing ' ' across the screen
void clearScreen() {
    std::string str = "";
    for (int r = 0; r < rOffset + screenSize; r++)
        str += std::string(cOffset + screenSize * 2 + 1, ' ') + '\n';
    reset();
    printf(str.c_str());
}

// Runs animation for "loading" a screen somewhere in the window
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

// Runs animation for "closing" a screen somewhere in the window
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

// Prints the menu located under the screen
// Calling with save == 1 adds the coordinates of each menu item to menuPos
void printMenu(int save) {
    const int usableScreen = (cOffset * 2) + (screenSize * 2) - (menuCOffset * 2);
    int gaps[MENU_SIZE];
    gaps[1] = std::round((usableScreen - ((MENU_SIZE + 1) * ((float)(menu[0].length() + menu[1].length()) / 2))) / (MENU_SIZE + 1));
    gaps[0] = std::round(gaps[1] + ((float)menu[1].length() / 2));

    //CONSOLE_SCREEN_BUFFER_INFO* info = (CONSOLE_SCREEN_BUFFER_INFO*)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    CONSOLE_SCREEN_BUFFER_INFO* info = new CONSOLE_SCREEN_BUFFER_INFO;
    
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

// Sets the window to the desired width in the given number of milliseconds
void changeWindow(int width, int height, int time) {
    time *= 0.9;
    int sleepTime = 0, increments[] = {INT_MAX, INT_MAX};
    if (width == windowWidth) {
        for (int i = 20; i < std::abs(height - windowHeight); i++) {
            for (int j = 1; j < increments[1] && i * j <= std::abs(height - windowHeight); j++) {
                if (((float)std::abs(height - windowHeight) / j) * i > time * 0.95 && ((float)std::abs(height - windowHeight) / j) * i < time * 1.05) {
                    sleepTime = i;
                    increments[1] = j;
                }
            }
        }
        increments[0] = 0;
        increments[1] *= windowHeight < height ? 1 : -1;
    } else if (height == windowHeight) {
        for (int i = 20; i < std::abs(width - windowWidth); i++) {
            for (int j = 1; j < increments[0] && i * j <= std::abs(width - windowWidth); j++) {
                if (((float)std::abs(width - windowWidth) / j) * i > time * 0.95 && ((float)std::abs(width - windowWidth) / j) * i < time * 1.05) {
                    sleepTime = i;
                    increments[0] = j;
                }
            }
        }
        increments[0] *= windowWidth < width ? 1 : -1;
        increments[1] = 0;
    } else {
        for (int i = 20; i < std::abs(width - windowWidth); i++) {
            for (int j = 1; j < increments[0] && i * j <= std::abs(width - windowWidth); j++) {
                if (((float)std::abs(width - windowWidth) / j) * i > time * 0.6 && ((float)std::abs(width - windowWidth) / j) * i < time * 1.4) {
                    sleepTime = i;
                    increments[0] = j;
                }
            }
        }
        increments[0] *= windowWidth < width ? 1 : -1;
        for (int j = 1; j < increments[1] && sleepTime * j <= std::abs(height - windowHeight); j++) {
            if (((float)std::abs(height - windowHeight) / j) * sleepTime > time * 0.6 && ((float)std::abs(height - windowHeight) / j) * sleepTime < time * 1.4)
                increments[1] = j;
        }
        increments[1] *= windowHeight < height ? 1 : -1;
    }
    if (sleepTime == 0 && increments[0] == INT_MAX)
        return;
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style |= WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    const bool increasingWidth = windowWidth < width;
    const bool increasingHeight = windowHeight < height;
    while ((increasingWidth && windowWidth < width) || (!increasingWidth && windowWidth > width) ||
                (increasingHeight && windowHeight < height) || (!increasingHeight && windowHeight > height)) {
        windowWidth += increments[0];
        windowHeight += increments[1];
        SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
        removeScrollbar();
        Sleep(sleepTime);
    }
    windowWidth = width;
    windowHeight = height;
    SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
}

// Prints the box for a conversation
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

// Loads given dialogue file, expands window, and displays conversation
void conversation(npcID npc) {
    setColor(0);
    changeWindow(CONVERSATION_WINDOW_WIDTH, windowHeight, 700);
    Sleep(500);
    screenLoad(convoSize, screenSize, conversationROffset, conversationCOffset);
    Sleep(500);

    json dial = json::parse(std::ifstream("Dialogues/dialogue.json"))[npc.id];
    for (int i = 0; i < dial.size(); i++) {
        if (Pos(dial[i]["r"].template get<int>(), dial[i]["c"].template get<int>()) == npc.pos) {
            dial = dial[i]["lines"];
            break;
        }
    }
    const SMALL_RECT scrollRect {
        conversationCOffset + 2,
        conversationROffset + 1,
        conversationCOffset + 7 + maxCharsConvo,
        conversationROffset + 13
    };
    const SMALL_RECT clipRect {
        conversationCOffset + 2,
        conversationROffset,
        conversationCOffset + 7 + maxCharsConvo,
        conversationROffset + 13
    };
    const CHAR_INFO charInfo {
        ' ',
        0
    };
    for (int i = 0, r = 0; i < dial.size(); i++) {
        if (2 * i + r > 12) {
            ScrollConsoleScreenBuffer(hConsole, &scrollRect, &clipRect, {conversationCOffset + 2, conversationROffset - 1}, &charInfo);
            r -= 2;
        }

        std::string line = dial[i]["line"].template get<std::string>();
        setCursor(conversationROffset + 2 * i + r + 1, conversationCOffset + 2);
        const Pos pos = dial[i]["player"].template get<bool>() ? *playerPos : npc.pos;
        const Cell cell = frames[currFrame][pos.r][pos.c];
        setColorCell(cell);
        for (int j = 0; j < 2; j++)
            printf("%c", cell.value);
        setColor(0);
        printf(" :");
        Sleep(500);
        
        std::vector<std::string> strs;
        std::stringstream stream(line);
        std::string token;
        std::string newLine = "";
        while (getline(stream, token, ' ')) {
            if (newLine.length() + token.length() > maxCharsConvo) {
                if (token.length() > maxCharsConvo) {
                    const int len = maxCharsConvo - newLine.length() - 1;
                    newLine += " " + token.substr(0, len) + "-";
                    token.erase(0, len);
                    strs.push_back(newLine);
                    while (token.length() > maxCharsConvo) {
                        strs.push_back(token.substr(0, maxCharsConvo - 1) + "-");
                        token.erase(0, maxCharsConvo - 1);
                    }
                } else {
                    strs.push_back(newLine);
                }
                newLine = token;
            } else {
                newLine += (newLine.length() == 0 ? "" : " ") + token;
            }
        }
        strs.push_back(newLine);

        for (int j = 0; j < (int)strs.size(); j++) {
            r += (j ? 1 : 0);
            if (2 * i + r > 12) {
                ScrollConsoleScreenBuffer(hConsole, &scrollRect, &clipRect, {conversationCOffset + 2, conversationROffset}, &charInfo);
                r--;
            }
            setCursor(conversationROffset + 2 * i + r + 1, conversationCOffset + 7);
            printConversationText(strs[j]);
        }
        Sleep(500);
    }
    Sleep(1000);
    screenClose(convoSize, screenSize, conversationROffset, conversationCOffset);
    changeWindow(ORIGINAL_WINDOW_WIDTH, windowHeight, 700);
}

// Prints a line of conversation dialogue character by character
void printConversationText(std::string line) {
    for (char c : line) {
        printf("%c", c);
        Sleep(20);
    }
}