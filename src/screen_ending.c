#include "raylib.h"
#include "screens.h"
#include <math.h>
#include <stdlib.h>

#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048

const int PIXEL_ART_TILE_SIZE = 3;
//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int noiseOffsetX1 = 0;
static int noiseOffsetY1 = 0;
static int noiseOffsetX2 = 0;
static int noiseOffsetY2 = 0;
static int noiseOffsetX3 = 0;
static int noiseOffsetY3 = 0;

float noiseScale1 = 0; // large details
float noiseScale2 = 0; // middle details
float noiseScale3 = 0; // micro details

static Camera2D camera = {0};

Texture2D mapTexture = {0};

static float deltaTime = 0.0f;
//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void InitEndingScreen(void)
{
    // TODO: Initialize ENDING screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    // Random offsets for a unique map result every launch
    noiseOffsetX1 = GetRandomValue(0, 9999);
    noiseOffsetY1 = GetRandomValue(0, 9999);
    noiseOffsetX2 = GetRandomValue(0, 9999);
    noiseOffsetY2 = GetRandomValue(0, 9999);
    noiseOffsetX3 = GetRandomValue(0, 9999);
    noiseOffsetY3 = GetRandomValue(0, 9999);

    
    // A larger value = finer and more frequent details ("noise").
    // A smaller value = larger and smoother shapes.
    noiseScale1 = 20.0f / 4;  // Low frequency for large "continents"
    noiseScale2 = 50.0f / 4;  // Medium frequency for "hills" and "islands"
    noiseScale3 = 100.0f / 4; // High frequency for fine "roughness"

    // --- Pre-generate noise layers ---
    Image noiseImage1 = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, noiseOffsetX1, noiseOffsetY1, noiseScale1);
    Image noiseImage2 = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, noiseOffsetX2, noiseOffsetY2, noiseScale2);
    Image noiseImage3 = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, noiseOffsetX3, noiseOffsetY3, noiseScale3);

    // Create the final image to store our pixel-art map
    Image pixelArtMapImage = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLANK);

    // --- Generate the final map ---
    for (int y = 0; y < MAP_HEIGHT; y += PIXEL_ART_TILE_SIZE)
    {
        for (int x = 0; x < MAP_WIDTH; x += PIXEL_ART_TILE_SIZE)
        {
            Color noiseColor1 = GetImageColor(noiseImage1, x, y);
            Color noiseColor2 = GetImageColor(noiseImage2, x, y);
            Color noiseColor3 = GetImageColor(noiseImage3, x, y);

            float val1 = (float)noiseColor1.r / 255.0f;
            float val2 = (float)noiseColor2.r / 255.0f;
            float val3 = (float)noiseColor3.r / 255.0f;

            float finalValue = val1 * 0.55f + val2 * 0.35f + val3 * 0.10f;

            Color terrainColor;

            if (finalValue < 0.30f)
                terrainColor = (Color){20, 40, 100, 255}; // Deep Water
            else if (finalValue < 0.38f)
                terrainColor = (Color){40, 80, 150, 255}; // Shallow Water
            else if (finalValue < 0.42f)
                terrainColor = (Color){230, 210, 130, 255}; // Sand/Beach
            else if (finalValue < 0.55f)
                terrainColor = (Color){100, 180, 70, 255}; // Grassland
            else if (finalValue < 0.70f)
                terrainColor = (Color){50, 140, 50, 255}; // Forest/Dark Grass
            else if (finalValue < 0.85f)
                terrainColor = (Color){120, 100, 80, 255}; // Mountain/Rock
            else
                terrainColor = (Color){180, 180, 170, 255}; // Snow/Peak

            ImageDrawRectangle(&pixelArtMapImage, x, y, PIXEL_ART_TILE_SIZE, PIXEL_ART_TILE_SIZE, terrainColor);
        }
    }

    UnloadImage(noiseImage1);
    UnloadImage(noiseImage2);
    UnloadImage(noiseImage3);

    mapTexture = LoadTextureFromImage(pixelArtMapImage);
    UnloadImage(pixelArtMapImage);

    // Setup camera
    camera.target = (Vector2){(float)MAP_WIDTH / 2.0f, (float)MAP_HEIGHT / 2.0f};
    camera.offset = (Vector2){(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

// Ending Screen Update logic
void UpdateEndingScreen(void)
{
    deltaTime = GetFrameTime();

    // --- Update ---
    // Camera control
    if (IsKeyDown(KEY_RIGHT))
        camera.target.x += 400.0f * deltaTime / camera.zoom;
    if (IsKeyDown(KEY_LEFT))
        camera.target.x -= 400.0f * deltaTime / camera.zoom;
    if (IsKeyDown(KEY_UP))
        camera.target.y -= 400.0f * deltaTime / camera.zoom;
    if (IsKeyDown(KEY_DOWN))
        camera.target.y += 400.0f * deltaTime / camera.zoom;

    // Camera zoom with mouse wheel (smooth adjustment)
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0)
    {
        camera.zoom = expf(logf(camera.zoom) + (wheelMove * 0.1f));
    }

    // Restrict zoom
    if (camera.zoom > 3.0f)
        camera.zoom = 3.0f;
    else if (camera.zoom < 0.4f)
        camera.zoom = 0.4f;


    // Calculate half of the visible width/height in world coordinates
    float halfVisibleWidth = (float)GetScreenWidth() / 2.0f / camera.zoom;
    float halfVisibleHeight = (float)GetScreenHeight() / 2.0f / camera.zoom;

    // Define min and max values for camera.target.x
    // Left edge of the map + half visible width
    float minTargetX = halfVisibleWidth;
    // Right edge of the map - half visible width
    float maxTargetX = (float)MAP_WIDTH - halfVisibleWidth;

    // Define min and max values for camera.target.y
    // Top edge of the map + half visible height
    float minTargetY = halfVisibleHeight;
    // Bottom edge of the map - half visible height
    float maxTargetY = (float)MAP_HEIGHT - halfVisibleHeight;

    // Apply limits to camera.target
    // Clamp is a function that restricts a value between a minimum and maximum.
    camera.target.x = fmaxf(minTargetX, fminf(camera.target.x, maxTargetX));
    camera.target.y = fmaxf(minTargetY, fminf(camera.target.y, maxTargetY));

    // Additional condition: if the map is smaller than the window, center it
    // (this is important so the camera doesn't "get stuck" on the edge if the zoom is too large)
    if (MAP_WIDTH < GetScreenWidth() / camera.zoom)
    {
        camera.target.x = (float)MAP_WIDTH / 2.0f;
    }
    if (MAP_HEIGHT < GetScreenHeight() / camera.zoom)
    {
        camera.target.y = (float)MAP_HEIGHT / 2.0f;
    }

    if (IsKeyDown(KEY_ENTER)){
        finishScreen = 1;
        PlaySound(fxCoin); 
    }
}

// Ending Screen Draw logic
void DrawEndingScreen(void)
{
    ClearBackground(BLACK);

    BeginMode2D(camera);
    DrawTexture(mapTexture, 0, 0, WHITE);
    EndMode2D();

    DrawText(TextFormat("Zoom: %.2fx", camera.zoom), 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Target: (%.0f, %.0f)", camera.target.x, camera.target.y), 10, 40, 20, RAYWHITE);
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 70, 20, RAYWHITE);
    DrawText("Controls: mouse wheel, arrows, ENTER to end", 10, 100, 20, RAYWHITE);
}

// Ending Screen Unload logic
void UnloadEndingScreen(void)
{
    // TODO: Unload ENDING screen variables here!
    UnloadTexture(mapTexture);
}

int FinishEndingScreen(void)
{
    return finishScreen;
}