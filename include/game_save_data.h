
#ifndef GAME_SAVE_DATA_H
#define GAME_SAVE_DATA_H

#include <stdbool.h>

typedef struct GameSaveData {
    int highScore;         
    int totalCoins;       
    bool characterUnlocked[5]; 
    float lastPlayTime;     
} GameSaveData;

void SaveGameData(const char* filename, GameSaveData* data);
void LoadGameData(const char* filename, GameSaveData* data);

#endif // GAME_SAVE_DATA_H