#include "my_utils.h"
#include "raylib.h"
#include "game_save_data.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//----------------------------------------------------------------------------------
// Timer functions
//----------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------
// Save Game Data functions
//----------------------------------------------------------------------------------

void SaveGameData(const char* filename, GameSaveData* data){
    FILE *file;
    file = fopen(filename, "wb");

    if (file == NULL){
        TraceLog(LOG_FATAL, "Failed to open data file to write");
        exit(EXIT_FAILURE);
    }

    fwrite(data, sizeof(GameSaveData), 1, file);
    fclose(file);

}


void LoadGameData(const char* filename, GameSaveData* data){
    FILE *file;
    file = fopen(filename, "rb");
    // file not found (first start)
    if (file == NULL){
        TraceLog(LOG_WARNING, "Save file not found, start initialisation...");
        data->highScore = 0;
        memset(data->characterUnlocked, 0, sizeof(data->characterUnlocked));
        data->lastPlayTime = 0.0f;
        return;
    }

    if (fread(data, sizeof(GameSaveData), 1, file) != 1){
        // file not readable, set to default
        data->highScore = 0;
        data->lastPlayTime = 0.0f;
        memset(data->characterUnlocked, 0, sizeof(data->characterUnlocked));
    }
    fclose(file);
    TraceLog(LOG_WARNING, "Loading game data - success!");
}





#if defined(_WIN32)
    #include <direct.h> // Для _mkdir
    #define MKDIR(path) _mkdir(path)
    #define PATH_SEPARATOR "\\"
#else
    #include <sys/stat.h> // Для mkdir
    #define MKDIR(path) mkdir(path, 0755) // Права доступа: rwxr-xr-x
    #define PATH_SEPARATOR "/"
#endif

// Функция для получения полного пути к файлу сохранения
// Возвращает указатель на статический буфер, используйте результат сразу после вызова.
const char* GetSaveFilePath(void)
{
    static char fullPath[1024] = { 0 };
    char dirPath[1024] = { 0 };
    const char *appName = "RaylibHobby";
    const char *fileName = "savegame.bin";

#if defined(_WIN32)
    // Windows: %APPDATA%\RaylibHobby
    const char* appDataPath = getenv("APPDATA");
    if (appDataPath != NULL) {
        snprintf(dirPath, sizeof(dirPath), "%s%s%s", appDataPath, PATH_SEPARATOR, appName);
    } else {
        // Запасной вариант, если %APPDATA% не найдена
        snprintf(dirPath, sizeof(dirPath), ".%s%s", PATH_SEPARATOR, appName);
    }
#elif defined(__linux__)
    // Linux: ~/.local/share/RaylibHobby
    const char* homeDir = getenv("HOME");
    if (homeDir != NULL) {
        snprintf(dirPath, sizeof(dirPath), "%s%s.local%sshare%s%s", homeDir, PATH_SEPARATOR, PATH_SEPARATOR, PATH_SEPARATOR, appName);
    } else {
        // Запасной вариант
        snprintf(dirPath, sizeof(dirPath), ".%s%s", PATH_SEPARATOR, appName);
    }
#elif defined(__APPLE__)
    // macOS: ~/Library/Application Support/RaylibHobby
    const char* homeDir = getenv("HOME");
    if (homeDir != NULL) {
        snprintf(dirPath, sizeof(dirPath), "%s%sLibrary%sApplication Support%s%s", homeDir, PATH_SEPARATOR, PATH_SEPARATOR, PATH_SEPARATOR, appName);
    } else {
        // Запасной вариант
        snprintf(dirPath, sizeof(dirPath), ".%s%s", PATH_SEPARATOR, appName);
    }
#else
    // Другие платформы: сохраняем в подпапку в текущей директории
    snprintf(dirPath, sizeof(dirPath), ".%s%s", PATH_SEPARATOR, appName);
#endif

    // Создаем директорию для приложения, если она не существует.
    // Примечание: Этот простой подход предполагает, что родительские папки (например, .local/share) уже существуют.
    MKDIR(dirPath);

    // Добавляем имя файла к пути директории
    snprintf(fullPath, sizeof(fullPath), "%s%s%s", dirPath, PATH_SEPARATOR, fileName);

    return fullPath;
}




bool TouchLeft(void)  { int c=GetTouchPointCount(); if (c==0) return false; Vector2 p=GetTouchPosition(0); return p.x < (GetScreenWidth()/2); }
bool TouchRight(void) { int c=GetTouchPointCount(); if (c==0) return false; Vector2 p=GetTouchPosition(0); return p.x >= (GetScreenWidth()/2); }