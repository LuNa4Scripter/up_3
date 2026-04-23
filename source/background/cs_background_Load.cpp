#include "cs_background_Load.h"
#include <iostream>
#include <direct.h>

Texture2D backgroundTexture;

void LoadGameBackground() {
    char cwd[1024];
    _getcwd(cwd, sizeof(cwd));

    TraceLog(LOG_INFO, "=========================================");
    TraceLog(LOG_INFO, "DEBUG: CURRENT WORKING DIRECTORY: %s", cwd);
    TraceLog(LOG_INFO, "DEBUG: LOOKING FOR: %s/assets/background.png", cwd);
    TraceLog(LOG_INFO, "=========================================");

    backgroundTexture = LoadTexture("assets/background.png");

    if (backgroundTexture.id == 0) {
        TraceLog(LOG_WARNING, "FAILED: 'assets/background.png' not found. Trying '../assets/...'");
        backgroundTexture = LoadTexture("../assets/background.png");
    }

    if (backgroundTexture.id == 0) {
        TraceLog(LOG_ERROR, "CRITICAL ERROR: COULD NOT LOAD BACKGROUND FROM ANY PATH!");
    } else {
        TraceLog(LOG_INFO, "SUCCESS: BACKGROUND LOADED (ID: %i)", backgroundTexture.id);
    }
}

void UnloadGameBackground() {
    if (backgroundTexture.id != 0) {
        UnloadTexture(backgroundTexture);
        TraceLog(LOG_INFO, "DEBUG: Background texture unloaded.");
    }
}

void DrawGameBackground() {
    if (backgroundTexture.id != 0) {
        DrawTextureEx(backgroundTexture, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
    }
}