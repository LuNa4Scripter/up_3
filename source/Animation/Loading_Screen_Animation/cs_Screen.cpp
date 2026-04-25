#include "cs_Screen.h"
#include "../data/cs_data.h"
#include "cs_text_falling.h"
#include "cs_shaking.h"
#include "cs_decoration.h"
#include "cs_Create_TV.h"

void InitDiegeticUI();
void UpdateDiegeticUI(float dt);
void DrawDiegeticUI();

#define InitDecoration InitDiegeticUI
#define UpdateDecoration UpdateDiegeticUI
#define DrawDecoration DrawDiegeticUI

static LoadingStep currentStep = L_STATE_FALLING;
static float stepTimer = 0.0f;

void InitLoadingScreen() {
    currentStep = L_STATE_FALLING;
    stepTimer = 0.0f;
    InitTextFalling();
    InitDecoration();
    InitCreateTV();
}

void UpdateLoadingScreen(float dt) {
    stepTimer += dt;
    
    UpdateImGuiShake(dt);
    UpdateDecoration(dt);

    switch (currentStep) {
        case L_STATE_FALLING:
            UpdateTextFalling(dt);
            if (IsTextFallingFinished()) {
                currentStep = L_STATE_TV_BORN;
                stepTimer = 0.0f;
            }
            break;

        case L_STATE_TV_BORN:
            UpdateCreateTV(dt);
            if (IsTVAnimationFinished()) {
                state = MAP_SELECT;
            }
            break;
    }
}

void DrawLoadingScreen() {
    ClearBackground(CLR_BG);

    rlImGuiBegin();
        DrawDecoration();

        if (currentStep == L_STATE_FALLING) {
            DrawTextFalling(); 
        }
    rlImGuiEnd();

    if (currentStep == L_STATE_TV_BORN) {
        DrawCreateTV();
    }

    rlImGuiBegin();
        DrawLoadingImGui();
    rlImGuiEnd();
}

void DrawLoadingImGui() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));
    ImGui::Begin("LoadingOverlay", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoBackground | 
        ImGuiWindowFlags_NoInputs);

    ImGui::End();
}