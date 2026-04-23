#include "cs_shaking.h"
#include "raylib.h"

static float imguiShakeDuration = 0.0f;
static float imguiShakeIntensity = 0.0f;
static ImVec2 imguiShakeOffset = { 0.0f, 0.0f };

void StartImGuiShake(float duration, float intensity) {
    imguiShakeDuration = duration;
    imguiShakeIntensity = intensity;
}

void UpdateImGuiShake(float dt) {
    if (imguiShakeDuration > 0.0f) {
        imguiShakeDuration -= dt;
        float currentIntensity = imguiShakeIntensity * (imguiShakeDuration / 0.5f); 
        
        imguiShakeOffset.x = (float)GetRandomValue(-100, 100) / 100.0f * currentIntensity;
        imguiShakeOffset.y = (float)GetRandomValue(-100, 100) / 100.0f * currentIntensity;
    } 
    else {
        imguiShakeOffset = { 0.0f, 0.0f };
    }
}

ImVec2 GetImGuiShakeOffset() {
    return imguiShakeOffset;
}