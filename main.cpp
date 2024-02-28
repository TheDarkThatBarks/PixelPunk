#include "main.h"

std::string map;
std::vector<std::vector<int>> mapCoord;

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
    /*std::fflush(stdout);
    _setmode(_fileno(stdout), 0x00020000); // _O_U16TEXT
    std::wcout << L"Hello, ĐĄßĞĝ!\n";
    std::fflush(stdout);
    _setmode(_fileno(stdout), _O_TEXT);*/
    initMap(map);
    loadAnimation();
    keyPress();
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

void updateDisplay(int val, int oldR, int oldC, int newR, int newC) {
    if (oldR >= screenPos.r && oldR < screenPos.r + screenSize && oldC >= screenPos.c && oldC < screenPos.c + screenSize) {
        Pos oldPos = mapToScreen({oldR, oldC});
        setCursor(cOffset + (oldPos.c * 2), rOffset + oldPos.r);
        setColor(0);
        printf(oldPos.r == screenSize - 1 ? "__" : "  ");
    }
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
                            if (selection < MENU_SIZE - 1)
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