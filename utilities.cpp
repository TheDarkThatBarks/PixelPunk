#include "utilities.hpp"

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