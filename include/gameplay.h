#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "raylib.h"


typedef struct Player {
    Vector2 position;  
    float speed;      
} Player;


typedef struct EnvItem {
    Rectangle rect; 
    int blocking;   // 1 - block 0 - pass
    Color color;    
} EnvItem;


#endif // GAMEPLAY_H