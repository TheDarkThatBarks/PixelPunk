#include "mapEditor.hpp"

int main() {
    keyPress();
}

void keyPress() {
    int kbCode = 0;
    while (kbCode != KB_ESCAPE) {
        if (kbhit()) {
            kbCode = getch();
            if (kbCode == 0 || kbCode == 224) {

            } else if (kbCode == KB_SPACE) {

            }
        }
    }
}