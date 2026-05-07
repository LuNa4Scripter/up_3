#include "cs_player_heart_main.h"
#include "player_Heart_function_implementation/cs_implementation.h" 
#include "../UI_call/cs_player_Heart.h"

extern void DrawPlayerHeartImgui(float currentHp, float maxHp);

void InitPlayerHeart() {
    TriggerHeartUI();
}

void UpdatePlayerHeart() {
    float deltaTime = GetFrameTime();
    UpdatePlayerHeartLogic(deltaTime);
}

void DrawPlayerHeart() {
    DrawPlayerHeartImgui(100.0f, 100.0f); 
}