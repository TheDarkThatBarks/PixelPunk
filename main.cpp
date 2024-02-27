#include "main.h"
void screenLoad();

int main() {
    GetWindowRect(console, &r);
    windowWidth = 355;
    windowHeight = 407;
    MoveWindow(console, r.left, r.top, windowWidth, windowHeight, TRUE);
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    SetWindowPos(console, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    SetWindowTextW(console, L"Square RPG");

    //PlaySoundW(L"Beep Speech.wav", NULL, SND_FILENAME | SND_ASYNC);

    //std::ifstream m("C:/Users/ellys/source/repos/SquareRPG/map1.txt");
    std::ifstream m("map2.txt");
	map = std::string((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
    std::fflush(stdout);
    _setmode(_fileno(stdout), 0x00020000); // _O_U16TEXT
    std::wcout << L"Hello, ĐĄßĞĝ!\n";
    std::fflush(stdout);
    _setmode(_fileno(stdout), _O_TEXT);
    initMap(map);
    loadAnimation();
    keyPress();
}

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

Pos screenToMap(Pos pos) {
    pos.r += screenPos.r;
    pos.c += screenPos.c;
    return pos;
}

Pos mapToScreen(Pos pos) {
    pos.r -= screenPos.r;
    pos.c -= screenPos.c;
    return pos;
}

void initMap(std::string map) {
    rows = 1, cols = 0;
	for (int i = 0; i < map.length(); i++) {
		if (map.at(i) == '\n') {
			rows++;
            if (!cols)
                cols = i / 2;
        }
	}
    if (rows <= screenSize && cols <= screenSize)
        screenPos = {0, 0};
    screenPos = {0, 0};
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
                    val = PLAYER;
                    playerPos->r = r;
                    playerPos->c = c / 2;
                    break;
                case '1':
                    screenPos.r = r;
                    screenPos.c = c / 2;
                case '*':
                    val = WALL;
                    break;
                case '-': {
                    val = ENEMY;
                    Pos* p = (Pos*)malloc(sizeof(Pos));
                    p->r = r;
                    p->c = c / 2;
                    enemyPos.push_back(p);
                    break;
                } case '0':
                    screenPos.r = r;
                    screenPos.c = c / 2;
                    break;
            }
            mapCoord[r][c / 2] = val;
        }
        r++;
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

void loadAnimation() {
    std::vector<void (*)()> funcs;

    Sleep(500);
    expandWindow(1000, 700);
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

void updateDisplay(int val, int oldR, int oldC, int newR, int newC) {
    Pos oldPos = mapToScreen({oldR, oldC});
    setCursor(cOffset + (oldPos.c * 2), rOffset + oldPos.r);
    setColor(0);
    printf(oldPos.r == screenSize - 1 ? "__" : "  ");
    Pos newPos = mapToScreen({newR, newC});
    setCursor(cOffset + (newPos.c * 2), rOffset + newPos.r);
    setColor(val);
    printf("  ");
    reset();
}

void changePos(int val, Pos* pos, int newR, int newC) {
    if (newR >= 0 && newR < rows && newC >= 0 && newC < cols && mapCoord[newR][newC] != PLAYER && mapCoord[newR][newC] >= 0) {
        mapCoord[pos->r][pos->c] = 0;
        mapCoord[newR][newC] = val;
        int rChange = 0, cChange = 0;
        if (val == PLAYER) {
            if (newR >= screenPos.r + screenSize - screenThreshold && screenPos.r + screenSize < rows) {
                rChange++;
            } else if (screenPos.r - 1 >= 0 && newR < screenPos.r + screenThreshold) {
                rChange--;
            } else if (newC >= screenPos.c + screenSize - screenThreshold && screenPos.c + screenSize < cols) {
                cChange++;
            } else if (screenPos.c - 1 >= 0 && newC < screenPos.c + screenThreshold) {
                cChange--;
            }
        }
        if (rChange + cChange != 0) {
            screenPos.r += rChange;
            screenPos.c += cChange;
            printScreen();
        } else if (newR >= screenPos.r && newR < screenPos.r + screenSize && newC >= screenPos.c && newC < screenPos.c + screenSize) {
            updateDisplay(val, pos->r, pos->c, newR, newC);
        }
        pos->r = newR;
        pos->c = newC;
    }
}

void keyPress() {
    int kbCode = 0;
    while (kbCode != KB_ESCAPE) {
        if (kbhit()) {
            kbCode = getch();
            if (kbMode == MOVE) {
                if (kbCode == 0 || kbCode == 224) {
                    int rChange = 0, cChange = 0;
                    switch (getch()) {
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
                    changePos(PLAYER, playerPos, playerPos->r + rChange, playerPos->c + cChange);
                    enemyAI();
                } else if (kbCode == KB_SPACE) {
                    enemyAI();
                } else if (kbCode == KB_TAB) {
                    kbMode = MENU;
                    updateSelection();
                }
            } else if (kbMode == MENU) {
                if (kbCode == 0 || kbCode == 224) {
                    switch (getch()) {
                        case KB_LEFT:
                            if (selection > 0)
                                updateSelection('L');
                            break;
                        case KB_RIGHT:
                            if (selection < menuSize - 1)
                                updateSelection('R');
                            break;
                    }
                } else if (kbCode == KB_TAB) {
                    kbMode = MOVE;
                    printMenu(0);
                }
            }
        }
    }
}

Node* nodeInit(int r, int c, int f, int g, Node* prev) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->r = r;
    n->c = c;
    n->f = f;
    n->g = g;
    n->prev = prev;
    return n;
}

std::vector<Node*> createPath(Node* current) {
    std::vector<Node*> totalPath;
    totalPath.push_back(current);
    while (current->prev != NULL) {
        current = current->prev;
        totalPath.insert(totalPath.begin(), current);
    }
    return totalPath;
}

int heuristic(Node node) {
    return std::abs(node.r - playerPos->r) + std::abs(node.c - playerPos->c);
}

Node* findMin(std::vector<Node*> list) {
    Node* min = nodeInit(0, 0, INT_MAX, 0, NULL);
    for (Node* n : list) {
        if (n->f < min->f)
            min = n;
    }
    return min;
}

std::vector<Node*> pathfind(Node start, Node goal) {
    std::vector<Node*> openSet;
    std::vector<Pos> closedSet;
    start.g = 0;
    start.f = heuristic(start);
    openSet.push_back(&start);
    while (!openSet.empty()) {
        Node* current = findMin(openSet);
        if (*current == goal)
            return createPath(current);
        openSet.erase(std::find(openSet.begin(), openSet.end(), current));
        Pos pos = {current->r, current->c};
        if (std::find(closedSet.begin(), closedSet.end(), pos) != closedSet.end())
            continue;
        closedSet.push_back(pos);
        int options[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (int i = 0; i < 4; i++) {
            int newR = current->r + options[i][0];
            int newC = current->c + options[i][1];
            if (newR < 0 || newR == rows || newC < 0 || newC == cols || mapCoord[newR][newC] < 0)
                continue;
            Pos p = {newR, newC};
            if (std::find(closedSet.begin(), closedSet.end(), p) != closedSet.end())
                continue;
            Node* neighbor = nodeInit(newR, newC, INT_MAX, INT_MAX, current);
            for (Node* n : openSet) {
                if (*n == *neighbor) {
                    neighbor = n;
                    break;
                }
            }
            int newG = current->g + 1;
            if (newG < neighbor->g) {
                neighbor->prev = nodeInit(current->r, current->c, current->f, current->g, current->prev);
                neighbor->g = newG;
                neighbor->f = newG + heuristic(*neighbor);
                bool in = false;
                for (int j = 0; !in && j < openSet.size(); j++) {
                    if (*openSet[j] == *neighbor) {
                        openSet[j] = neighbor;
                        in = true;
                    }
                }
                if (!in)
                    openSet.push_back(neighbor);
            }
        }
    }
    return std::vector<Node*>();
}

void enemyAI() {
    for (Pos* pos : enemyPos) {
        std::vector<Node*> list = pathfind({pos->r, pos->c, INT_MAX, INT_MAX, NULL}, {playerPos->r, playerPos->c, 0, 0, NULL});
        if (!list.empty())
            changePos(ENEMY, pos, list[1]->r, list[1]->c);
    }
}

void printMenu(int save) {
    const int usableScreen = (cOffset * 2) + (screenSize * 2) - (menuCOffset * 2);
    int gaps[menuSize];
    gaps[1] = std::round((usableScreen - ((menuSize + 1) * ((float)(menu[0].length() + menu[1].length()) / 2))) / (menuSize + 1));
    gaps[0] = std::round(gaps[1] + ((float)menu[1].length() / 2));

    CONSOLE_SCREEN_BUFFER_INFO* info = (CONSOLE_SCREEN_BUFFER_INFO*)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    
    setCursor(menuCOffset, rOffset + screenSize + menuROffset);
    setColor(0);
    for (int i = 0; i < menuSize; i++) {
        if (i > 1)
            gaps[i] = std::round(gaps[i - 1] + ((float)(menu[i - 2].length() - menu[i].length()) / 2));
        if (save) {
            GetConsoleScreenBufferInfo(hConsole, info);
            menuPos.push_back({info->dwCursorPosition.Y, info->dwCursorPosition.X + gaps[i]});
        }
        printf("%s%s", std::string(gaps[i], ' ').c_str(), menu[i].c_str());
    }
}

void updateSelection() {
    setCursor(menuPos[selection].c, menuPos[selection].r);
    setColor("LIGHT_GRAY", "BLACK");
    printf(menu[selection].c_str());
    reset();
}

void updateSelection(char dir) {
    setCursor(menuPos[selection].c, menuPos[selection].r);
    setColor(0);
    printf(menu[selection].c_str());
    if (dir == 'L') {
        selection--;
    } else if (dir == 'R') {
        selection++;
    }
    updateSelection();
}

void conversation(std::string dialogue) {
    std::ifstream d(dialogue);
	currentDialogue = std::string((std::istreambuf_iterator<char>(d)), std::istreambuf_iterator<char>());
    std::vector<std::string> lines;
    size_t pos = 0;
    std::string delimiter = "\n";
    while ((pos = currentDialogue.find(delimiter)) != std::string::npos) {
        std::string str = currentDialogue.substr(0, pos);
        lines.push_back(str);
        currentDialogue.erase(0, pos + delimiter.length());
    }
    lines.push_back(currentDialogue);

    for (int i = 0; i < lines.size(); i++) {
        pos = lines[i].find(":");
        int speaker = std::stoi(lines[i].substr(0, pos));
        setCursor(conversationCOffset, conversationROffset + 2 * i);
        setColor(speaker);
        printf("  ");
        setColor(0);
        //printf(" : %s", lines[i].substr(pos + 1).c_str());
        printConversationText(lines[i].substr(pos + 1));
        Sleep(500);
    }
}

void printConversationText(std::string line) {
    printf(" : ");
    for (char c : line) {
        printf("%c", c);
        Sleep(10);
    }
}