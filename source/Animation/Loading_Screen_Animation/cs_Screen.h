#ifndef CS_SCREEN_H
#define CS_SCREEN_H

#include "raylib.h"
#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h" 

typedef enum {
    L_STATE_FALLING,
    L_STATE_SLICING,
    L_STATE_TV_BORN,
    L_STATE_TUNNEL,
    L_STATE_FINISHED    
} LoadingStep;

void InitLoadingScreen();
void UpdateLoadingScreen(float dt);
void DrawLoadingScreen();

void DrawLoadingImGui();

#endif