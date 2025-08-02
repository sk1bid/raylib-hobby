#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdbool.h>
#include "raylib.h"

typedef struct Timer {
    double startTime;   // Start time (seconds)
    double lifeTime;    // Lifetime (seconds)
} Timer;

void StartTimer(Timer *timer, double lifetime);
bool TimerDone(Timer timer);
double GetElapsed(Timer timer);

// Draw your text in the center of the screen, you can choose y coordinate.
void DrawTextCentered(const char* text, int fontSize, int posY, Color color);

const char* GetSaveFilePath(void);

bool TouchLeft(void);
bool TouchRight(void);
#endif // UTILS_H