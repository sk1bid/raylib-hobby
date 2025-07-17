
/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Title Screen Functions Definitions (Init, Update, Draw, Unload)
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
#include "my_utils.h"

#define NUM_FRAMES 4
#define MAX_BUTTONS 2

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static float frameWidth = 0.0f;
static float frameHeight = 0.0f;
static float spriteScale = 0.0f;

static float startButtonPosY = 0;
static Rectangle startBtnSourceRec = {0};
static Rectangle startBtnBounds = {0};

static float exitButtonPosY = 0;
static Rectangle exitBtnSourceRec = {0};
static Rectangle exitBtnBounds = {0};

static Button buttons[MAX_BUTTONS] = {0};

static Vector2 mousePoint = {0};

static int currentBtn = 0; // number of button

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    SetTargetFPS(60);
    ShowCursor();
    spriteScale = 2.0f;
    frameHeight = 64.0f;
    frameWidth = 64.0f;
    // TODO: Initialize TITLE screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    startButtonPosY = 3.0f * frameHeight;                                                        // go to the 4 row in sprite
    startBtnSourceRec = (Rectangle){64 * 4, startButtonPosY + 15, frameWidth, frameHeight - 40}; // start point for our Frame
    startBtnBounds = (Rectangle){GetScreenWidth() / 2.0f - frameWidth, GetScreenHeight() / 2.0f - frameHeight / 2.0f, frameWidth * 2, (frameHeight / 2.5f) * 2};

    exitButtonPosY = 4.0f * frameHeight;                                                       // go to the 5 row in sprite
    exitBtnSourceRec = (Rectangle){64 * 4, exitButtonPosY + 15, frameWidth, frameHeight - 40}; // start point for our Frame
    exitBtnBounds = (Rectangle){GetScreenWidth() / 2.0f - frameWidth, GetScreenHeight() / 2.0f + frameHeight / 2.0f, frameWidth * 2, (frameHeight / 2.5f) * 2};

    for (int i = 0; i < MAX_BUTTONS; i++)
    {
        buttons[i].action = false;
        buttons[i].state = 0;
    }

    mousePoint = (Vector2){0.0f, 0.0f};

    currentBtn = 0;
    buttons[0].state = 1;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    mousePoint = GetMousePosition();
    
    for (int i = 0; i < MAX_BUTTONS; i++)
    {
        buttons[i].action = false;
    }

    if (IsKeyDown(KEY_ENTER))
    {
        if (currentBtn == 0)
        {
            finishScreen = 2; // play game
            buttons[0].action = true;
            buttons[0].state = 2;
        }
        else if (currentBtn == 1)
        { // close game
            finishScreen = 1;
            buttons[0].action = true;
            buttons[0].state = 2;
        }
    }

    if (CheckCollisionPointRec(mousePoint, startBtnBounds))
    {
        currentBtn = -1;
        for (int i = 0; i < MAX_BUTTONS; i++){
            buttons[i].state = 0;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            buttons[0].state = 2;
        }
        else
        {
            buttons[0].state = 1;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            buttons[0].action = true;
            finishScreen = 2;
        }
    }
    else if (CheckCollisionPointRec(mousePoint, exitBtnBounds))
    {
        currentBtn = -1;
        for (int i = 0; i < MAX_BUTTONS; i++){
            buttons[i].state = 0;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            buttons[1].state = 2;
        }
        else
        {
            buttons[1].state = 1;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            buttons[1].action = true;
            finishScreen = 1;
        }
    }

    else if (IsKeyPressed(KEY_DOWN))
    {
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            buttons[i].state = 0;
        }
        currentBtn++;
        if (currentBtn > MAX_BUTTONS - 1)
        {
            currentBtn = 0;
        }
        buttons[currentBtn].state = 1;
    }
    else if (IsKeyPressed(KEY_UP))
    {
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            buttons[i].state = 0;
        }
        currentBtn--;
        if (currentBtn < 0)
        {
            currentBtn = MAX_BUTTONS - 1;
        }
        buttons[currentBtn].state = 1;
    }
    else if (currentBtn == -1)
    {
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            buttons[i].state = 0;
        }
        buttons[0].state = 1;
        currentBtn = 0;
    }

    for (int i = 0; i < MAX_BUTTONS; i++)
    {
        if (buttons[i].action)
        {
            PlaySound(fxCoin);
        }
    }

    startBtnSourceRec.x = buttons[0].state * frameWidth + frameWidth * 4;
    startBtnSourceRec.y = startButtonPosY + 15;

    exitBtnSourceRec.x = buttons[1].state * frameWidth + frameWidth * 4;
    exitBtnSourceRec.y = exitButtonPosY + 15;
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // TODO: Draw TITLE screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(LIME, 0.5));
    DrawTextCentered("DOODLER", 70, 200, BLACK);
    DrawTexturePro(
        pixelButtonsTexture,
        startBtnSourceRec,
        startBtnBounds,
        (Vector2){0, 0},
        0.0f,
        WHITE);
    DrawTexturePro(
        pixelButtonsTexture,
        exitBtnSourceRec,
        exitBtnBounds,
        (Vector2){0, 0},
        0.0f,
        WHITE);
    //DrawRectangleLinesEx(startBtnBounds, 2, RED);
    //DrawRectangleLinesEx(exitBtnBounds, 2, RED);
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}
