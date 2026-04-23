#include "cs_text_falling.h"
#include "../../data/cs_data.h"
#include "../../include/imgui.h"
#include "cs_shaking.h"
#include <math.h>
#include <stdio.h>

static float textPosY = -250.0f;
static float targetY = SCREEN_HEIGHT / 2.0f;
static float velocityY = 0.0f;
static float slashProgress = 0.0f;
static bool isFinished = false;
static bool hasShaked = false;
static float slashAlpha = 0.0f;
static float settleTimer = 0.0f;

void InitTextFalling() {
    textPosY = -250.0f;
    velocityY = 0.0f;
    slashProgress = 0.0f;
    slashAlpha = 0.0f;
    settleTimer = 0.0f;
    isFinished = false;
    hasShaked = false;
}

void UpdateTextFalling(float dt) {
    if (textPosY < targetY) {
        velocityY += 5000.0f * dt;
        textPosY += velocityY * dt;
        if (textPosY >= targetY) {
            textPosY = targetY;
            if (!hasShaked) {
                StartImGuiShake(0.4f, 40.0f);
                hasShaked = true;
            }
        }
    } else {
        if (settleTimer < 1.5f) {
            settleTimer += dt;
            if ((int)(settleTimer * 15) % 2 == 0) {
                float drift = (1.5f - settleTimer) * 5.0f;
                textPosY = targetY + (float)GetRandomValue(-1, 1) * drift;
            }
        } else {
            slashProgress += dt;
            if (slashProgress > 0.0f && slashProgress < 0.3f) slashAlpha = 1.0f;
            else slashAlpha -= 1.5f * dt;
            if (slashAlpha < 0) slashAlpha = 0;
            if (slashProgress > 2.0f) isFinished = true;
        }
    }
}

void DrawTextFalling() {
    ImVec2 shake = GetImGuiShakeOffset();
    ImGui::SetNextWindowPos(ImVec2(shake.x, shake.y));
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));
    ImGui::Begin("WestTextLayer", nullptr, 
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const char* tL = "WE";
    const char* tR = "ST";
    float fSize = 160.0f;
    float scale = (fSize / ImGui::GetFontSize());
    ImVec2 sL = ImGui::CalcTextSize(tL);
    sL.x *= scale; sL.y *= scale;

    float p = slashProgress;

    auto DrawBrokenText = [&](ImVec2 pos, const char* text, float dir) {
        float burst = (p > 0.1f) ? (p - 0.1f) * 1600.0f : 0.0f;
        float fade = (p > 0.1f) ? (1.0f - (p - 0.1f) * 2.5f) : 1.0f;
        if (fade < 0) fade = 0;

        if (p > 0.05f) {
            for (int i = 0; i < 10; i++) {
                float ox = (float)GetRandomValue(-25, 25) * p;
                float oy = (float)GetRandomValue(-25, 25) * p;
                drawList->AddText(ImGui::GetFont(), fSize, ImVec2(pos.x + ox, pos.y + oy), ImColor(0, 255, 245, (int)(100 * fade)), text);
            }
        }

        ImVec2 finalPos = ImVec2(pos.x + (dir * burst), pos.y + (burst * 0.05f * dir));
        drawList->AddText(ImGui::GetFont(), fSize, ImVec2(finalPos.x + 2, finalPos.y + 2), ImColor(0, 255, 245, (int)(180 * fade)), text);
        drawList->AddText(ImGui::GetFont(), fSize, finalPos, ImColor(255, 255, 255, (int)(255 * fade)), text);
    };

    float centerOff = 5.0f;
    if (p > 0.1f) centerOff += (p - 0.1f) * 900.0f;

    ImVec2 base = ImVec2(SCREEN_WIDTH / 2.0f, textPosY);

    DrawBrokenText(ImVec2(base.x - sL.x - centerOff, base.y - sL.y / 2.0f), tL, -1.0f);
    DrawBrokenText(ImVec2(base.x + centerOff, base.y - sL.y / 2.0f), tR, 1.0f);

    if (p > 0.0f) {
        ImVec2 c = ImVec2(SCREEN_WIDTH / 2.0f, targetY);
        float l = 2000.0f;
        float angleX = 14.0f; 
        ImVec2 s = ImVec2(c.x + l / angleX, c.y - l / 2.0f);
        ImVec2 e = ImVec2(c.x - l / angleX, c.y + l / 2.0f);

        drawList->AddLine(s, e, ImColor(255, 255, 255, (int)(255 * slashAlpha)), 40.0f * slashAlpha);
        
        for (int i = 0; i < 10; i++) {
            float o = (float)GetRandomValue(-50, 50);
            drawList->AddLine(ImVec2(s.x + o, s.y), ImVec2(e.x + o, e.y), ImColor(0, 255, 245, (int)(160 * slashAlpha)), 2.5f);
        }

        if (p < 0.15f) StartImGuiShake(0.45f, 100.0f);
    }

    ImGui::End();
}

bool IsTextFallingFinished() {
    return isFinished;
}