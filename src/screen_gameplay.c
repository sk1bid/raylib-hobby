

#include "raylib.h"
#include "screens.h"
#include <math.h>

#define MAX_BUILDINGS 150
//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static Vector2 player = {0};
static float ballSpeed = 0;

static Rectangle buildings[MAX_BUILDINGS] = {0};
static Color buildColors[MAX_BUILDINGS] = {0};
static int spacing = 0;

static int screenWidth = 0;
static int screenHeight = 0;

static int isCursorHiden = 0;

static float deltaTime = 0.0f;

static Camera2D camera = {0};

static Vector2 mousePos = {0};

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    spacing = 0;

    player = (Vector2){(float)screenWidth / 2, 310};
    ballSpeed = 300.0f; // pixels per second

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        buildings[i].width = (float)GetRandomValue(50, 200);
        buildings[i].height = (float)GetRandomValue(100, 800);
        buildings[i].x = -6000 + spacing;
        buildings[i].y = screenHeight - 130.0f - buildings[i].height;
        
        spacing += (int)buildings[i].width;

        buildColors[i] = (Color){GetRandomValue(100, 240), GetRandomValue(100, 240), GetRandomValue(100, 240), 255}; // rgbA
    }

    camera.target = (Vector2){player.x, player.y};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    isCursorHiden = 0;

    mousePos = GetMousePosition();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    mousePos = GetMousePosition();


    // TODO: Update GAMEPLAY screen variables here!
    deltaTime = GetFrameTime();

    // Arrows controling
    if (IsKeyDown(KEY_D))
        player.x += deltaTime * ballSpeed;
    if (IsKeyDown(KEY_A))
        player.x -= deltaTime * ballSpeed;

    if (IsKeyPressed(KEY_ENTER)){
        finishScreen = 1;
    }
    camera.target = (Vector2){player.x, player.y};

    camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove() * 0.1f)); // for smooth scaling
    if (camera.zoom > 3.0f)
    {
        camera.zoom = 3.0f;
    }
    else if (camera.zoom < 0.1f)
    {
        camera.zoom = 0.1f;
    }

    HideCursor();
    isCursorHiden = 1;
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    BeginMode2D(camera);

    DrawRectangle(-6000, 320, 13000, 8000, DARKGRAY);
    for (int i = 0; i < MAX_BUILDINGS; i++)
        DrawRectangleRec(buildings[i], buildColors[i]);
    DrawCircleV(player, 10, BLUE);

    EndMode2D();

    DrawCircleV(mousePos, 10, GREEN); // use cursor position
    DrawText("1) Use arrows to move the ball", 0, 50, 20, BLACK);
    DrawText("3) Use your mouse wheel to zoom", 0, 100, 20, BLACK);

    // Правильно отображаем мировые координаты с помощью %.0f или %.2f
    DrawText(TextFormat("Wx: %.0f, Wy: %.0f", GetScreenToWorld2D(mousePos, camera).x, GetScreenToWorld2D(mousePos, camera).y), mousePos.x + 20.0f, mousePos.y - 20.0f, 20, BLACK);
}
// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}