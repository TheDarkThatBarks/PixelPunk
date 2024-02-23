#include "main.h"

int main() {
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 355, 375, TRUE);
    removeScrollbar();
    //std::ifstream m("C:/Users/ellys/source/repos/SquareRPG/map1.txt");
    std::ifstream m("map2.txt");
	std::string map((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
    initMap(map);
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
    screenPos = {8, 8};
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
                case '*':
                    val = WALL;
                    break;
                case '-':
                    val = ENEMY;
                    Pos* p = (Pos*)malloc(sizeof(Pos));
                    p->r = r;
                    p->c = c / 2;
                    enemyPos.push_back(p);
                    break;
            }
            mapCoord[r][c / 2] = val;
        }
        r++;
    }
    printScreen();
    reset();
}

void printScreen() {
    setCursor(cOffset, rOffset - 1);
    for (int c = 0; c < screenSize; c++)
        std::cout << "__";
    for (int r = 0; r < screenSize; r++) {
        setCursor(cOffset - 1, rOffset + r);
        setColor(0);
        std::cout << "|";
        for (int c = 0; c < screenSize; c++) {
            Pos pos = screenToMap({r, c});
            setColor(mapCoord[pos.r][pos.c]);
            std::cout << (r == screenSize - 1 ? "__" : "  ");
        }
        setColor(0);
        std::cout << "|";
    }
}

void updateDisplay(int val, int oldR, int oldC, int newR, int newC) {
    Pos oldPos = mapToScreen({oldR, oldC});
    setCursor(cOffset + (oldPos.c * 2), rOffset + oldPos.r);
    setColor(0);
    std::cout << (oldPos.r == screenSize - 1 ? "__" : "  ");
    Pos newPos = mapToScreen({newR, newC});
    setCursor(cOffset + (newPos.c * 2), rOffset + newPos.r);
    setColor(val);
    std::cout << "  ";
    reset();
}

void changePos(int val, Pos* pos, int newR, int newC) {
    if (newR >= 0 && newR < rows && newC >= 0 && newC < cols && mapCoord[newR][newC] != PLAYER && mapCoord[newR][newC] >= 0) {
        mapCoord[pos->r][pos->c] = 0;
        mapCoord[newR][newC] = val;
        if (val == PLAYER && newR >= screenPos.r + screenSize - screenThreshold && screenPos.r + 1 + screenSize <= rows) {
            screenPos.r++;
            printScreen();
        } else if (val == PLAYER && newR < screenPos.r + screenThreshold && screenPos.r - 1 >= 0) {
            screenPos.r--;
            printScreen();
        } else if (val == PLAYER && newC >= screenPos.c + screenSize - screenThreshold && screenPos.c + 1 + screenSize <= cols) {
            screenPos.c++;
            printScreen();
        } else if (val == PLAYER && newC < screenPos.c + screenThreshold && screenPos.c - 1 >= 0) {
            screenPos.c--;
            printScreen();
        } else if (newR >= screenPos.r && newR < screenPos.r + screenSize && newC >= screenPos.c && newC < screenPos.c + screenSize) {
            updateDisplay(val, pos->r, pos->c, newR, newC);
        }
        pos->r = newR;
        pos->c = newC;
    }
}

void keyPress() {
    int kbCode;
    while (kbCode != KB_ESCAPE) {
        if (kbhit()) {
            kbCode = getch();
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
    start.g = 0;
    start.f = heuristic(start);
    openSet.push_back(&start);
    while (!openSet.empty()) {
        Node* current = findMin(openSet);
        if (*current == goal)
            return createPath(current);
        openSet.erase(std::find(openSet.begin(), openSet.end(), current));
        int options[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (int i = 0; i < 4; i++) {
            int newR = current->r + options[i][0];
            int newC = current->c + options[i][1];
            if (newR < 0 || newR == rows || newC < 0 || newC == cols || mapCoord[newR][newC] < 0)
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
    std::vector<Node*> v;
    return v;
}

void enemyAI() {
    for (Pos* pos : enemyPos) {
        std::vector<Node*> list = pathfind({pos->r, pos->c, INT_MAX, INT_MAX, NULL}, {playerPos->r, playerPos->c, 0, 0, NULL});
        changePos(ENEMY, pos, list[1]->r, list[1]->c);
    }
}