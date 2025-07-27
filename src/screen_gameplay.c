/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "gameplay.h"
#include <stdlib.h>
#include "my_utils.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Color playerColor = {0};
static float highestPlatformY = 0;

static float gameCoeff = 0;
static int  playerScore = 0;
static float deltaTime = 0.0f;
Player doodler = {0};
Camera2D camera = {0};
EnvItem envItems[MAX_PLATFORMS] = {0};
Vector2 globalBottomScreen = {0};
Vector2 globalTopScreen = {0};
Color playerColors[] = {
        YELLOW, GOLD, ORANGE, PINK, RED, MAROON,
        GREEN, LIME, DARKGREEN, SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE,
        BEIGE, BROWN, DARKBROWN, WHITE, BLACK
    };
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------


int  UpdatePlayer(Player *player, EnvItem *_envItems, float delta);

void Update_Camera_Push_Bounds(Camera2D *Camera, Player *player, int width, int height);


void UpdateAndRecyclePlatforms(Camera2D camera, EnvItem* envItems);

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    
    
    gameCoeff = 0.1f;
    playerScore = 0;
    playerColor = RED;
    camera = (Camera2D){0};
    camera.target = (Vector2){(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    camera.offset = (Vector2){(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    envItems[0] = (EnvItem){{0, (float)GetScreenHeight() - 20, (float)GetScreenWidth(), 20}, 1, BROWN};
    
    highestPlatformY = envItems[0].rect.y;

    for (int i = 1; i < MAX_PLATFORMS; i++)
    {
        
        highestPlatformY -= GetRandomValue(80, 150); 
        
        envItems[i] = (EnvItem){
            {GetRandomValue(0, GetScreenWidth() - PLATFORM_WIDTH), highestPlatformY, PLATFORM_WIDTH, PLATFORM_HEIGHT},
            1, BROWN
        };
    }
    
    
    globalBottomScreen = (Vector2){0};
    globalTopScreen = (Vector2){0};
    
    doodler = (Player){0};
    doodler.speed = -850.0f;
    doodler.collaider = (Rectangle){(float)GetScreenWidth() / 2.0f - 20, envItems[0].rect.y + 50, 40, 40};
    
    framesCounter = 0;
    finishScreen = 0;
    SetTargetFPS(0);
    
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!
    deltaTime = GetFrameTime();
    if (UpdatePlayer(&doodler, envItems, deltaTime) == 1){
        finishScreen = 1;
    }
    UpdateAndRecyclePlatforms(camera, envItems);
    Update_Camera_Push_Bounds(&camera, &doodler, GetScreenWidth(), GetScreenHeight());
    // Press enter or tap to change to ENDING screen
    

}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    ClearBackground(LIGHTGRAY);

    BeginMode2D(camera);

        for (int i = 0; i < MAX_PLATFORMS; i++){
            DrawRectangleRec(envItems[i].rect, envItems[i].color);
        }

        
        DrawRectangleRec(doodler.collaider, playerColor);
    
    EndMode2D();
    DrawText(TextFormat("Score: %d", playerScore), 10, 20, 30, Fade(BLACK, 0.8));
    
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    if (playerScore > gameData.highScore){
        gameData.highScore = playerScore;
    }
    diePos = (Vector3){doodler.collaider.x, doodler.collaider.y, doodler.speed};
    SaveGameData(GetSaveFilePath(), &gameData);
    return finishScreen;
}

int UpdatePlayer(Player *player, EnvItem *_envItems, float delta){
    if (IsKeyDown(KEY_A) || TouchLeft()){
        player->collaider.x -= PLAYER_HOR_SPEED * delta;
    }
    if (IsKeyDown(KEY_D) || TouchRight()){
        player->collaider.x += PLAYER_HOR_SPEED * delta;
    }
    
    if (player->collaider.x > GetScreenWidth()){
        player->collaider.x = -player->collaider.width;
    }
    else if (player->collaider.x < -player->collaider.width){
        player->collaider.x = GetScreenWidth();
    }

    player->speed += G * delta; // gravitation, 
    float potential_new_y = player->collaider.y + player->speed * delta;
    Rectangle playerFutureRect = { player->collaider.x, 
                                 potential_new_y, 
                                 player->collaider.width, 
                                 player->collaider.height };

    bool hitPlatform = false;
    float corrected_y = potential_new_y; // Изначально предполагаем, что не будет коллизии
    for (int i = 0; i < MAX_PLATFORMS; i++){
        EnvItem *ei = _envItems + i; // iterate platforms
        if (ei->blocking && player->speed >= 0 &&
        CheckCollisionRecs(playerFutureRect, ei->rect) && (player->collaider.y + player->collaider.height) <= ei->rect.y)
        { 
            hitPlatform = true;
            player->speed = -PLAYER_JUMP_SPEED;
            playerColor = RED;
            corrected_y = ei->rect.y - player->collaider.height; 
            break;
        }
        
    }
    player->collaider.y = corrected_y; 

    if ((int)player->collaider.y < 0 && abs((int)player->collaider.y - GetScreenHeight()) > playerScore){
        playerScore = abs((int)player->collaider.y - GetScreenHeight());
    }
    else if ((int)player->collaider.y > 0  && GetScreenHeight() - (int)player->collaider.y > playerScore){
        playerScore =  GetScreenHeight() - (int)player->collaider.y;
    }
    if (hitPlatform){
        PlaySound(fxJump);
    }

    globalBottomScreen =  GetScreenToWorld2D((Vector2){0, GetScreenHeight()}, camera);
    globalTopScreen =  GetScreenToWorld2D((Vector2){0, 0}, camera);

    if (player->collaider.y  > globalBottomScreen.y && player->collaider.y < GetScreenHeight() - 100){
        return 1;

    }

    return 0;

   
}

void Update_Camera_Push_Bounds(Camera2D *Camera, Player *player, int width, int height){
    // Камера всегда смещена так, чтобы её цель (target) находилась по центру экрана.
    Camera->offset = (Vector2){ (float)width/2.0f, (float)height/2.0f };
    if (player->collaider.y < Camera->target.y){
        Camera->target.y = player->collaider.y;
    }
    
    
}

void UpdateAndRecyclePlatforms(Camera2D _camera, EnvItem* _envItems){
    globalBottomScreen = GetScreenToWorld2D((Vector2){0, GetScreenHeight()}, _camera);

    for (int i = 1; i < MAX_PLATFORMS; i++){
        if (_envItems[i].rect.y > globalBottomScreen.y){
            highestPlatformY -= GetRandomValue(80, 150);
            _envItems[i].rect.y = highestPlatformY;
            _envItems[i].rect.x = GetRandomValue(0, GetScreenWidth() - PLATFORM_WIDTH);
        }
    }
}