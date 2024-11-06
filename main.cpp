#pragma comment (lib, "User32.lib")
#include "main.hpp"
//#include <locale>

std::vector<std::string> mapText;
int textIndex = 0;

// Creates and sizes window, loads map file, runs start animation, and starts game loop
int main() {
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, windowWidth, windowHeight, TRUE);
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    SetWindowPos(console, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    SetWindowTextW(console, L"PixelPunk");

    //PlaySoundW(L"Beep Speech.wav", NULL, SND_FILENAME | SND_ASYNC);

    //std::ifstream m("C:/Users/ellys/source/repos/SquareRPG/map1.txt");
    //std::ifstream m("map3.txt");
    std::ifstream m("Maps/gameDemo.txt");
	map = std::string((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
    /*std::fflush(stdout);
    _setmode(_fileno(stdout), 0x00020000); // _O_U16TEXT
    std::locale::global(std::locale(""));
    std::wcout << L"Hello, ĐĄßĞĝ!\n";
    std::wcout << L"⎽⎼⎻⎺";
    //std::fflush(stdout);
    _setmode(_fileno(stdout), _O_TEXT);*/
    std::srand(std::time(0));
    initMap(map);
    //getch();
    loadAnimation();
    keyPress();
    /*std::thread thr(animLoop);
    gameLoop();
    thr.join();*/
    return 0;
}

bool quit = false;

void gameLoop() {
    int kbCode = 0;
    while (kbCode != KB_ESCAPE) {
        //auto start = std::chrono::system_clock::now();
        if (kbhit()) {
            kbCode = getch();
            if (kbCode == 0 || kbCode == 224) {
                int rChange = 0, cChange = 0;
                int dir = -1;
                switch (getch()) {
                    case KB_UP:
                        rChange--;
                        dir = 0;
                        break;
                    case KB_DOWN:
                        rChange++;
                        dir = 1;
                        break;
                    case KB_LEFT:
                        cChange--;
                        dir = 2;
                        break;
                    case KB_RIGHT:
                        cChange++;
                        dir = 3;
                        break;
                }
                while (kbhit())
                    getch();
                changePos(playerPos, playerPos->r + rChange, playerPos->c + cChange * 2, true);
                enemyAI();
                updateScreen(dir);
            } else if (kbCode == 96 /* ` */) {
                std::vector<float> times;
                for (int n = 0; n < 1000000; n++) {
                    auto start = std::chrono::system_clock::now();
                    int rChange = 0, cChange = 0;
                    int dir = std::rand() % 4;
                    switch (dir) {
                        case 0:
                            rChange--;
                            break;
                        case 1:
                            rChange++;
                            break;
                        case 2:
                            cChange--;
                            break;
                        case 3:
                            cChange++;
                            break;
                    }
                    changePos(playerPos, playerPos->r + rChange, playerPos->c + cChange * 2, true);
                    //enemyAI();
                    updateScreen(dir);
                    times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count());
                }
                double sum = 0;
                for (double t : times)
                    sum += t;
                reset();
                std::cout << "                    ";
                reset();
                std::cout << (double)sum / (int)times.size();
            }
        }
        /*auto end = std::chrono::system_clock::now();
        auto loopTime = end - start;
        reset();
        std::cout << "                         ";
        reset();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(loopTime).count();
        if (loopTime < std::chrono::milliseconds(50))
            Sleep(50 - std::chrono::duration_cast<std::chrono::milliseconds>(loopTime).count());*/
    }
    quit = true;
}

void animLoop() {
    std::chrono::duration<double> elapsed = std::chrono::duration<double>::zero();
    int count = 0;
    while (!quit) {
        /*reset();
        printf("loop %d", count);
        count++;*/
        auto start = std::chrono::system_clock::now();
        /*if (elapsed >= std::chrono::milliseconds(500)) {
            currFrame = -currFrame + 1;
            for (int i = 0; i < (int)animChangeList.size(); i++) {
                Pos pos = animChangeList[i];
                if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
                    int c = pos.c;
                    std::vector<WORD> colors;
                    colors.push_back(computeColor(frames[currFrame][pos.r][pos.c]));
                    std::vector<char> buffer;
                    buffer.push_back(computePrintVal(pos, mapToScreen(pos)));
                    for (int j = 1; i + j < (int)animChangeList.size(); j++) {
                        if (animChangeList[i + j].r == pos.r && animChangeList[i + j].c == c + 1) {
                            c++;
                            colors.push_back(computeColor(frames[currFrame][pos.r][c]));
                            buffer.push_back(computePrintVal({pos.r, c}, mapToScreen({pos.r, c})));
                        } else {
                            break;
                        }
                    }
                    DWORD written;
                    WriteConsoleOutputAttribute(hConsole, &colors[0], c - pos.c + 1, {(short)(cOffset + pos.c), (short)(rOffset + pos.r)}, &written);
                    WriteConsoleOutputCharacterA(hConsole, &buffer[0], c - pos.c + 1, {(short)(cOffset + pos.c), (short)(rOffset + pos.r)}, &written);
                    i += c - pos.c;
                }
            }
            //reset();
            //std::cout << "                     ";
            //reset();
            //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            elapsed = std::chrono::duration<double>::zero();
        }// else {
            elapsed += std::chrono::system_clock::now() - start;
        //}*/
        if (elapsed >= std::chrono::milliseconds(500)) {
            currFrame = -currFrame + 1;
            //for (Pos pos : animChangeList) {
            for (int i = 0; i < (int)animChangeList.size(); i++) {
                Pos pos = animChangeList[i];
                if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
                    printCell(pos, mapToScreen(pos));
                    /*int c = pos.c;
                    std::vector<WORD> colors;
                    colors.push_back(computeColor(frames[currFrame][pos.r][pos.c]));
                    std::vector<char> buffer;
                    buffer.push_back(computePrintVal(pos, mapToScreen(pos)));
                    for (int j = 1; i + j < (int)animChangeList.size(); j++) {
                        if (animChangeList[i + j].r == pos.r && animChangeList[i + j].c == c + 1) {
                            c++;
                            colors.push_back(computeColor(frames[currFrame][pos.r][c]));
                            buffer.push_back(computePrintVal({pos.r, c}, mapToScreen({pos.r, c})));
                        } else {
                            break;
                        }
                    }
                    DWORD written;
                    WriteConsoleOutputAttribute(hConsole, &colors[0], c - pos.c + 1, {(short)(cOffset + pos.c), (short)(rOffset + pos.r)}, &written);
                    WriteConsoleOutputCharacterA(hConsole, &buffer[0], c - pos.c + 1, {(short)(cOffset + pos.c), (short)(rOffset + pos.r)}, &written);
                    i += c - pos.c;*/
                }
            }
            //reset();
            elapsed = std::chrono::duration<double>::zero();
        }
        elapsed += std::chrono::system_clock::now() - start;
    }
}

// Initializes a Cell struct
void initCell(Cell c) {
    c.type = ' ';
    c.value = 0;
    c.fore = 0;
    c.back = 0;
    c.isPlayer = false;
    c.isEnemy = false;
    c.isNPC = false;
    c.isStart = false;
}

// Copies Cell struct
Cell copyCell(Cell c) {
    Cell newCell;
    newCell.type = c.type;
    newCell.value = c.value;
    newCell.fore = c.fore;
    newCell.back = c.back;
    newCell.isPlayer = c.isPlayer;
    newCell.isEnemy = c.isEnemy;
    newCell.isNPC = c.isNPC;
    newCell.isStart = c.isStart;
    return newCell;
}

// Initializes game map using map file (which is encoded using the level editor, see MapEditor/Editor.html)
// Goes through map file, adding cells to the frames 2D vector, only adding a different cell to the second frame if it is different from the first frame
// Stores screen start position, player start position, enemy start positions, and NPC positions and IDs
void initMap(std::string map) {
    playerPos->r = -1;
    playerPos->c = -1;
    screenPos.r = -1;
    screenPos.c = -1;
    rows = std::stoi(map.substr(0, (int)map.find("x")));
    cols = std::stoi(map.substr(3, (int)map.find("|")));
    int idx = (int)map.find("|") + 1, r = 0;
    struct cell2 {
        char type;
        Cell cell;
    };
    cell2 lastCell = {
        0,
        {
            ' ',
            0,
            0,
            0,
            false,
            false,
            false,
            false
        }
    };
    initCell(lastCell.cell);
    frames[0] = std::vector<std::vector<Cell>>(rows, std::vector<Cell>());
    frames[1] = std::vector<std::vector<Cell>>(rows, std::vector<Cell>());
    bool start = false;
    //reset();
    //std::cout << idx << " " << rows << " " << cols << "\n";
    while (idx < (int)map.length()) {
        const char type = map[idx];
        //std::cout << (type == ' ' ? 'E' : type) << "\n";
        if (type == '^') {
            //std::cout << idx << "\n";
            start = true;
            idx++;
            continue;
        } else if (type == '|') {
            if (lastCell.type != 0 && lastCell.type != '*')
                frames[1][r].push_back(copyCell(lastCell.cell));
            idx++;
            r++;
            //std::cout << idx << " " << map.length() << " " << r << "\n";
            lastCell.type = 0;
            initCell(lastCell.cell);
            continue;
        }
        const std::string cellStr = map.substr(idx + 1, 3);
        //std::cout << "cellStr " << cellStr << "\n";
        cell2 cell = {
            type,
            {
                type,
                type == '-' ? ' ' : cellStr[0],
                std::stoi(std::string(1, cellStr[1]), 0, 16),
                std::stoi(std::string(1, cellStr[2]), 0, 16),
                type == '+',
                type == '-',
                type == '!',
                start
            }
        };
        //std::cout << cell.type << "," << cell.cell.value << "," << cell.cell.fore << "," << cell.cell.back << "," << cell.cell.isPlayer << "," << cell.cell.isNPC << "," << cell.cell.isStart << "\n";
        if (cell.cell.isNPC) {
            //std::cout << "NPC r: " << r << " " << frames[0].size() << "\n";
            npcIDs.push_back({
                {
                    r,
                    (int)frames[0][r].size()
                },
                cellStr
            });
        }
        start = false;
        if (type == '*') {
            /*animChangeList.push_back({
                r,
                (int)frames[0][r].size() - 1
            });*/
            if (animChangeList.empty()) {
                //std::cout << "Anim List r: " << r << " " << frames[0].size() << "\n";
                animChangeList.push_back({
                    r,
                    (int)frames[0][r].size() - 1
                });
            } else {
                for (int i = 0; i < animChangeList.size(); i++) {
                    if ((animChangeList[i].r == r && animChangeList[i].c > (int)frames[0][r].size() - 1) || animChangeList[i].r > r) {
                        //std::cout << "Anim List r: " << r << " " << frames[0].size() << "\n";
                        animChangeList.insert(animChangeList.begin() + i, {
                            r,
                            (int)frames[0][r].size() - 1
                        });
                        break;
                    } else if (i + 1 == animChangeList.size()) {
                        //std::cout << "Anim List r: " << r << " " << frames[0].size() << "\n";
                        animChangeList.push_back({
                            r,
                            (int)frames[0][r].size() - 1
                        });
                        break;
                    }
                }
            }
            //cell.cell.type = lastCell.cell.type;
            cell.cell.isPlayer = lastCell.cell.isPlayer;
            cell.cell.isEnemy = lastCell.cell.isEnemy;
            if (lastCell.cell.isNPC) {
                cell.cell.isNPC = lastCell.cell.isNPC;
                cell.cell.value = lastCell.cell.value;
                cell.cell.fore = lastCell.cell.fore;
            }
            cell.cell.isStart = lastCell.cell.isStart;
            frames[1][r].push_back(copyCell(cell.cell));
        } else {
            //std::cout << "Pushing 1 r: " << r << " " << frames[0].size() << "\n";
            //if (r == 50)
            //    std::cout << "Pushing 1 r: " << frames[0][r - 1].size() << "\n";
            frames[0][r].push_back(copyCell(cell.cell));
            if (cell.cell.isPlayer && playerPos->r == -1) {
                playerPos->r = r;
                playerPos->c = frames[0][r].size() - 1;
            }
            if (cell.cell.isEnemy && frames[0][r][frames[0][r].size() - 2].type != '-')
                enemyPos.push_back(new EnemyPos(new Pos(r, (int)frames[0][r].size() - 1), cellStr[0]));
            if (cell.cell.isStart && screenPos.r == -1) {
                //std::cout << "screenPos: " << r << " " << frames[0][r].size() << "\n";
                screenPos.r = r;
                screenPos.c = frames[0][r].size() - 1;
            }
            //std::cout << "Pushing 2 r: " << r << " " << frames[1].size() << "\n";
            if (lastCell.type != 0 && lastCell.type != '*')
                frames[1][r].push_back(copyCell(lastCell.cell));
        }
        lastCell = {
            cell.type,
            copyCell(cell.cell)
        };
        idx += 4;
    }
    if (lastCell.type != 0 && lastCell.type != '*')
        frames[1][r].push_back(copyCell(lastCell.cell));
    /*for (int i = 0; i < 2; i++) {
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < cols; k++)
                std::cout << frames[i][j][k].type << "," << frames[i][j][k].value << "," << frames[i][j][k].fore << "," << frames[i][j][k].back << "," << frames[i][j][k].isPlayer << "," << frames[i][j][k].isNPC << "," << frames[i][j][k].isStart << "\n";
        }
    }*/
}

// Changes the position of the player or an enemy
// If the player reaches a certain distance from the edge of the screen, will shift the screen on the next animation frame
// If the player does not reach that distance, will add moved positions to a reprint list
void changePos(Pos* pos, int newR, int newC, bool player) {
    if (newR >= 0 && newR < rows && newC >= 0 && newC < cols && frames[currFrame][newR][newC].type == ' ') {
        int rChange = 0, cChange = 0;
        if (player) {
            for (int i = 0; i < 2; i++) {
                frames[i][pos->r][pos->c].type = ' ';
                frames[i][pos->r][pos->c + 1].type = ' ';
                frames[i][newR][newC].type = '+';
                frames[i][newR][newC + 1].type = '+';
            }
            if (newR >= screenPos.r + screenSize - screenThreshold && screenPos.r + screenSize < rows) {
                rChange++;
            } else if (screenPos.r - 1 >= 0 && newR < screenPos.r + screenThreshold) {
                rChange--;
            } else if (newC >= screenPos.c + screenSize * 2 - screenThreshold * 2 && screenPos.c + screenSize * 2 < cols) {
                cChange++;
            } else if (screenPos.c - 1 >= 0 && newC < screenPos.c + screenThreshold * 2) {
                cChange--;
            }
            //reset();
            //printf("%d %d %d %d %d %d", pos->r, pos->c, newR, newC, rChange, cChange);
        } else {
            for (int i = 0; i < 2; i++) {
                frames[i][pos->r][pos->c].type = ' ';
                frames[i][pos->r][pos->c + 1].type = ' ';
                frames[i][newR][newC].type = '-';
                frames[i][newR][newC + 1].type = '-';
            }
        }
        if (rChange + cChange != 0) {
            reprint = {
                true,
                rChange,
                cChange * 2
            };
        } else if (newR >= screenPos.r && newR < screenPos.r + screenSize && newC >= screenPos.c && newC < screenPos.c + screenSize * 2) {
            redrawList.push_back({
                pos->r,
                pos->c
            });
            redrawList.push_back({
                newR,
                newC
            });
        }
        pos->r = newR;
        pos->c = newC;
    }
}

// Creates a line of '.' from given start position to player
void shoot(int startR, int startC) {
    bool toTheRight = startC >= playerPos->c + 2;
    bool usingCol = toTheRight || startC + 2 <= playerPos->c;
    std::vector<Projectile> list;

    startC += usingCol ? (toTheRight ? -1 : 2) : 0;
    startR += usingCol ? 0 : (playerPos->r > startR ? 1 : -1);

    int diffC = std::floor(startC - (playerPos->c + (toTheRight ? 2 : -1)));
    int diffR = std::floor(startR - playerPos->r);
    float slope = (float)diffR / diffC;
    for (int i = usingCol ? startC : startR;
         (usingCol && (i >= playerPos->c + 2 || i < playerPos->c)) || (!usingCol && (i < playerPos->r || i > playerPos->r));
         (usingCol && toTheRight) || (!usingCol && startR > playerPos->r) ? i-- : i++) {
        int r = (usingCol ? startR + (int)std::round(slope * (i - startC/*(toTheRight ? startC : 0)*/)) : i);
        int c = usingCol ? i : startC;
        list.push_back({mapToScreen({r, c}), '-'});
    }
    projectileList.insert(projectileList.end(), list.begin(), list.end());
}

void attack(Pos pos) {
    if (frames[currFrame][pos.r][pos.c].type == '-') {
        //EnemyPos* ePos = (EnemyPos*)malloc(sizeof(EnemyPos));
        int idx = -1;
        for (int i = 0; i < (int)enemyPos.size() && idx < 0; i++) {
            if (*(enemyPos[i]->pos) == pos)
                idx = i;
        }
        enemyPos.erase(enemyPos.begin() + idx);
        frames[currFrame][pos.r][pos.c].type = ' ';
        frames[currFrame][pos.r][pos.c + 1].type = ' ';
        printCell(pos, mapToScreen(pos));
        printCell({pos.r, pos.c + 1}, mapToScreen({pos.r, pos.c + 1}));
    }
}

// Core game loop, checks for keyboard input and also handles animation frames
void keyPress() {
    int kbCode = 0;
    std::chrono::duration<double> elapsed = std::chrono::duration<double>::zero();
    while (kbCode != KB_ESCAPE) {
        auto start = std::chrono::system_clock::now();
        if (kbhit()) {
            kbCode = getch();
            if (kbMode == MOVE) {
                if (kbCode == 0 || kbCode == 224) {
                    int rChange = 0, cChange = 0;
                    int dir = -1;
                    switch (getch()) {
                        case KB_UP:
                            rChange--;
                            dir = 0;
                            break;
                        case KB_DOWN:
                            rChange++;
                            dir = 1;
                            break;
                        case KB_LEFT:
                            cChange--;
                            dir = 2;
                            break;
                        case KB_RIGHT:
                            cChange++;
                            dir = 3;
                            break;
                    }
                    changePos(playerPos, playerPos->r + rChange, playerPos->c + cChange * 2, true);
                    enemyAI();
                    updateScreen(dir);
                    //while (kbhit())
                    //    getch();
                } else if (kbCode == KB_SPACE) {
                    enemyAI();
                    updateScreen(-1);
                }/* else if (kbCode == KB_TAB) {
                    kbMode = MENU;
                    updateSelection();
                }*/ else if (kbCode == KB_ENTER) {
                    bool foundNPC = false;
                    Pos npcPos = {-1, -1};
                    const int options[4][2] = {{1, 0}, {-1, 0}, {0, 2}, {0, -2}};
                    for (int i = 0; !foundNPC && i < 4; i++) {
                        foundNPC = frames[currFrame][playerPos->r + options[i][0]][playerPos->c + options[i][1]].type == '!';
                        if (foundNPC)
                            npcPos = {playerPos->r + options[i][0], playerPos->c + options[i][1]};
                    }
                    if (foundNPC) {
                        for (npcID npc : npcIDs) {
                            if (npc.pos == npcPos) {
                                conversation(npc);
                                break;
                            }
                        }
                    }
                }/* else if (kbCode == 47 /* / *//*) {
                    std::ifstream m("Maps/gameDemo.txt");
	                map = std::string((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
                    initMap(map);
                    printScreen();
                }*/
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
                } else if (kbCode == KB_ENTER) {
                    //shoot(std::floor(screenPos.r + screenSize / 2.0), screenPos.c + screenSize * 2 - 1);
                    //shoot(screenPos.r, screenPos.c + screenSize * 2 - 1);
                    //shoot(screenPos.r + screenSize - 1, screenPos.c + screenSize * 2 - 1);
                    shoot(std::floor(screenPos.r + screenSize / 2.0), screenPos.c);
                    //shoot(screenPos.r, std::floor((screenPos.c + screenSize * 2) / 2));
                    //shoot(screenPos.r + screenSize - 1, std::floor((screenPos.c + screenSize * 2) / 2));
                } else if (kbCode == KB_SPACE) {
                    if (selection == 0) {
                        kbMode = ATTACK;
                        printMenu(0);
                    }
                }
            } else if (kbMode == ATTACK) {
                if (kbCode == 0 || kbCode == 224) {
                    Pos attackPos = {playerPos->r, playerPos->c};
                    switch (getch()) {
                        case KB_UP:
                            attackPos.r--;
                            break;
                        case KB_DOWN:
                            attackPos.r++;
                            break;
                        case KB_LEFT:
                            attackPos.c -= 2;
                            break;
                        case KB_RIGHT:
                            attackPos.c += 2;
                            break;
                    }
                    attack(attackPos);
                }
                kbMode = MOVE;
            }
        }
        elapsed += std::chrono::system_clock::now() - start;
        if (elapsed >= std::chrono::milliseconds(500)) {
            currFrame = -currFrame + 1;
            //for (Pos pos : animChangeList) {
            for (int i = 0; i < (int)animChangeList.size(); i++) {
                Pos pos = animChangeList[i];
                if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
                    printCell(pos, mapToScreen(pos));
                    /*int c = pos.c;
                    std::vector<WORD> colors;
                    colors.push_back(computeColor(frames[currFrame][pos.r][pos.c]));
                    std::vector<char> buffer;
                    buffer.push_back(computePrintVal(pos, mapToScreen(pos)));
                    for (int j = 1; i + j < (int)animChangeList.size(); j++) {
                        if (animChangeList[i + j].r == pos.r && animChangeList[i + j].c == c + 1) {
                            c++;
                            colors.push_back(computeColor(frames[currFrame][pos.r][c]));
                            buffer.push_back(computePrintVal({pos.r, c}, mapToScreen({pos.r, c})));
                        } else {
                            break;
                        }
                    }
                    DWORD written;
                    WriteConsoleOutputAttribute(hConsole, &colors[0], c - pos.c + 1, {(short)(cOffset + pos.c), (short)(rOffset + pos.r)}, &written);
                    WriteConsoleOutputCharacterA(hConsole, &buffer[0], c - pos.c + 1, {(short)(cOffset + pos.c), (short)(rOffset + pos.r)}, &written);
                    i += c - pos.c;*/
                }
            }
            reset();
            elapsed = std::chrono::duration<double>::zero();
        }
    }
}

// Initializes Node struct
Node* nodeInit(int r, int c, int f, int g, Node* prev) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->r = r;
    n->c = c;
    n->f = f;
    n->g = g;
    n->prev = prev;
    return n;
}

// Creates backwards path from given Node struct
std::vector<Node*> createPath(Node* current) {
    std::vector<Node*> totalPath;
    totalPath.push_back(current);
    while (current->prev != NULL) {
        current = current->prev;
        totalPath.insert(totalPath.begin(), current);
    }
    return totalPath;
}

// Calculates heuristic function for A* algorithm
int heuristic(Node node) {
    return std::abs(node.r - playerPos->r) + (std::abs(node.c - playerPos->c) / 2);
}

int heuristic2(Node node) {
    return std::round(std::abs(std::sqrt(std::pow(node.r - playerPos->r, 2) + std::pow((node.c - playerPos->c) / 2.0, 2)) - 3));
}

// Finds minimum cost Node from given list
Node* findMin(std::vector<Node*> list) {
    //Node* min = nodeInit(0, 0, INT_MAX, 0, NULL);
    Node* min = new Node(0, 0, INT_MAX, 0, NULL);
    for (Node* n : list) {
        if (n->f < min->f)
            min = n;
    }
    return min;
}

// Uses A* algorithm to calculate best path from start Node struct to goal Node struct
std::vector<Node*> pathfind(Node start, Node goal, int (*heuristic)(Node)) {
    std::vector<Node*> openSet;
    std::vector<Pos> closedSet;
    start.g = 0;
    //start.f = heuristic2(start);
    start.f = (*heuristic)(start);
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
        const int options[4][2] = {{1, 0}, {-1, 0}, {0, 2}, {0, -2}};
        for (int i = 0; i < 4; i++) {
            int newR = current->r + options[i][0];
            int newC = current->c + options[i][1];
            if (newR < 0 || newR == rows || newC < 0 || newC == cols || (frames[currFrame][newR][newC].type != ' ' && frames[currFrame][newR][newC].type != '+'))
                continue;
            Pos p = {newR, newC};
            if (std::find(closedSet.begin(), closedSet.end(), p) != closedSet.end())
                continue;
            //Node* neighbor = nodeInit(newR, newC, INT_MAX, INT_MAX, current);
            Node* neighbor = new Node(newR, newC, INT_MAX, INT_MAX, current);
            for (Node* n : openSet) {
                if (*n == *neighbor) {
                    neighbor = n;
                    break;
                }
            }
            int newG = current->g + 1;
            if (newG < neighbor->g) {
                //neighbor->prev = nodeInit(current->r, current->c, current->f, current->g, current->prev);
                neighbor->prev = new Node(current);
                neighbor->g = newG;
                neighbor->f = newG + heuristic(*neighbor);
                bool in = false;
                for (int j = 0; !in && j < (int)openSet.size(); j++) {
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

int count = 0;

// Calculates path to player for each enemy on map and moves them one step closer to player
void enemyAI() {
    for (EnemyPos* enemy : enemyPos) {
        std::vector<Node*> list;
        if (enemy->type == ' ') {
            list = pathfind({enemy->pos->r, enemy->pos->c, INT_MAX, INT_MAX, NULL}, {playerPos->r, playerPos->c, 0, 0, NULL}, &heuristic);
        } else if (enemy->type == '!') {
            int dist = std::abs(playerPos->r - enemy->pos->r) + std::abs(playerPos->c - enemy->pos->c) / 2;
            if (dist > 4 && dist < 8) {
                shoot(enemy->pos->r, enemy->pos->c);
                count++;
                reset();
                std::cout << "         SHOOT" << count;
                continue;
            }
            double vC = (enemy->pos->c - playerPos->c) / 2.0;
            double vR = enemy->pos->r - playerPos->r;
            double magV = std::sqrt(vC * vC + vR * vR);
            int aC = (int)std::round(playerPos->c + vC / magV * 8) / 2 * 2;
            int aR = (int)std::round(playerPos->r + vR / magV * 4);
            /*reset();
            printf("                                        ");
            reset();
            std::cout << enemy->pos->r << "," << playerPos->r << "," << aC << "," << aR << "\n";*/
            if (aC <= 0 || aC >= cols - 2) {
                if (aR <= 1) {
                    aR = 2;
                } else if (aR >= rows - 2) {
                    aR = rows - 3;
                } else {
                    aR += playerPos->r > enemy->pos->r ? -1 : (playerPos->r < enemy->pos->r ? 1 : (std::rand() % 2 == 0 ? -1 : 1));
                }
                aC = aC <= 0 ? 2 : (cols - 4);
            } else if (aR <= 0 || aR >= rows - 1) {
                if (aC >= cols - 4) {
                    aC = cols - 6;
                } else if (aC <= 2) {
                    aC = 4;
                } else {
                    aC += playerPos->c > enemy->pos->c ? -2 : (playerPos->c < enemy->pos->c ? 2 : (std::rand() % 2 == 0 ? -2 : 2));
                }
                aR = aR <= 0 ? 1 : (rows - 2);
            }
            //std::cout << vC << "," << vR << "," << magV << "," << aC << "," << aR;
            //Sleep(2000);
            //getch();
            /*reset();
            std::cout << enemy->pos->r << "," << enemy->pos->c << "," << aR << "," << aC;*/
            Pos p = {aR, aC};
            if (*(enemy->pos) == p) {
                shoot(enemy->pos->r, enemy->pos->c);
                continue;
            }
            list = pathfind({enemy->pos->r, enemy->pos->c, INT_MAX, INT_MAX, NULL}, {aR, aC, 0, 0, NULL}, &heuristic2);
            /*reset();
            std::cout << list.size();
            getch();
            reset();
            printf("       ");*/
        }
        if (list.size() >= 2) {
            /*if (list[1]->r == enemy->pos->r && list[1]->c == enemy->pos->c) {
                reset();
                std::cout << "DEBUG";
            } else {*/
                changePos(enemy->pos, list[1]->r, list[1]->c, false);
            //}
        }
    }
}

// Reprints current menu selection in selected color
void updateSelection() {
    setCursor(menuPos[selection].r, menuPos[selection].c);
    setColor("LIGHT_GRAY", "BLACK");
    printf(menu[selection].c_str());
    reset();
}

// Reprints current menu selection in non-selected color and changes selection
void updateSelection(char dir) {
    setCursor(menuPos[selection].r, menuPos[selection].c);
    setColor(0);
    printf(menu[selection].c_str());
    if (dir == 'L') {
        selection--;
    } else if (dir == 'R') {
        selection++;
    }
    updateSelection();
}
