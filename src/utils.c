#include "my_utils.h"
#include <stdbool.h>
#include "raylib.h"

void StartTimer(Timer *timer, double lifetime)
{
    timer->startTime = GetTime();
    timer->lifeTime = lifetime;
}

bool TimerDone(Timer timer)
{
    return GetTime() - timer.startTime >= timer.lifeTime;
}

double GetElapsed(Timer timer)
{
    return GetTime() - timer.startTime;
}

void DrawTextCentered(const char* text, int fontSize, int posY, Color color){
    int textWidth = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() / 2) - (textWidth / 2);
    DrawText(text, posX, posY, fontSize, color);
}