#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#define G 1000.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_HOR_SPEED 250.0f
#define MAX_PLATFORMS 100

#define PLATFORM_WIDTH 80
#define PLATFORM_HEIGHT 20

#include "raylib.h"
#include "raymath.h"

typedef struct Player {
    Rectangle collaider;
    float speed;      
} Player;


typedef struct EnvItem {
    Rectangle rect; 
    int blocking;   // 1 - block 0 - pass
    Color color;    
} EnvItem;


#endif // GAMEPLAY_H