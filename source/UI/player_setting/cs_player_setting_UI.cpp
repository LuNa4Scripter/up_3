#include "cs_player_setting_UI.h"
#include "cs_click.h"
#include "../../Animation/UI_Animation/Player_setting/cs_player_setting_animation.h"
#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h"
#include "../../data/cs_data.h"
#include <math.h>
#include <stdlib.h>

void DrawPlayerSettingUI() {
    if (!isSettingsOpen && settingAnimPercent <= 0.0f) return;

    static int currentTab = 0;
    static float timer = 0.0f;
    timer += GetFrameTime();

    ImDrawList* d = ImGui::GetForegroundDrawList();
    float ease = GetEaseOutBack(settingAnimPercent);
    
    ImVec2 uiSize = { 600.0f, 600.0f }; 
    float centerX = (SCREEN_WIDTH / 2.0f);
    float centerY = (SCREEN_HEIGHT / 2.0f);
    ImVec2 center = { centerX, centerY };
    float radius = 280.0f * ease;

    const char* tabs[] = { "FRM_PER_SEC", "SIGNAL_LATENCY", "DISCONNECT" };
    int tabCount = 3;

    if (ImGui::IsKeyPressed(ImGuiKey_W) || ImGui::IsKeyPressed(ImGuiKey_UpArrow)) currentTab = (currentTab - 1 + tabCount) % tabCount;
    if (ImGui::IsKeyPressed(ImGuiKey_S) || ImGui::IsKeyPressed(ImGuiKey_DownArrow)) currentTab = (currentTab + 1) % tabCount;
    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        if (currentTab == 2) { isSettingsOpen = false; isGamePaused = false; }
    }

    ImColor pBright = ImColor(150, 255, 50, (int)(255 * settingAnimPercent)); 
    ImColor pDim = ImColor(40, 80, 20, (int)(180 * settingAnimPercent));      
    ImColor pBg = ImColor(5, 15, 5, (int)(230 * settingAnimPercent));        

    d->AddCircleFilled(center, radius + 5, ImColor(40, 45, 40, (int)(255 * settingAnimPercent)));
    d->AddCircleFilled(center, radius, pBg);

    for(int i = 0; i < 12; i++) {
        float angle = i * (30.0f * DEG2RAD);
        d->AddLine(
            { center.x + cosf(angle) * (radius - 30), center.y + sinf(angle) * (radius - 30) },
            { center.x + cosf(angle) * radius, center.y + sinf(angle) * radius },
            pDim, 2.0f
        );
    }
    d->AddCircle(center, radius * 0.7f, pDim, 64, 1.0f);
    d->AddCircle(center, radius * 0.4f, pDim, 64, 1.0f);

    float scanLine = fmodf(timer * 2.0f, PI * 2.0f);
    d->AddLine(center, { center.x + cosf(scanLine) * radius, center.y + sinf(scanLine) * radius }, ImColor(100, 255, 0, 80), 3.0f);

    for (int i = 0; i < tabCount; i++) {
        float angle = (i * 35.0f - 125.0f) * DEG2RAD; 
        ImVec2 tPos = { center.x + cosf(angle) * (radius * 0.65f), center.y + sinf(angle) * (radius * 0.65f) };
        
        bool isSelected = (currentTab == i);
        float flicker = (isSelected && (rand() % 100 > 95)) ? 0.5f : 1.0f;
        ImColor col = isSelected ? pBright : pDim;

        if (isSelected) {
            d->AddCircleFilled(tPos, 6.0f, pBright);
            d->AddCircle(tPos, 12.0f + sinf(timer * 5.0f) * 3.0f, pBright, 20, 1.5f);
        }

        const char* valStr = "";
        if(i == 0) valStr = TextFormat("FPS: %d", GetFPS());
        else if(i == 1) valStr = TextFormat("MS: %.2f", GetFrameTime() * 1000.0f);
        else valStr = "LEAVE";

        d->AddText(nullptr, 26.0f, { tPos.x + 25, tPos.y - 13 }, col, valStr);
    }

    ImVec2 oscPos = { center.x - 100, center.y + 100 };
    for (int i = 0; i < 50; i++) {
        float noise = (float)(rand() % 10) * 0.5f;
        float py1 = sinf(timer * 15.0f + i * 0.3f) * (15.0f + noise);
        float py2 = sinf(timer * 15.0f + (i + 1) * 0.3f) * (15.0f + noise);
        
        d->AddLine(
            { oscPos.x + i * 4, oscPos.y + py1 },
            { oscPos.x + (i + 1) * 4, oscPos.y + py2 },
            pBright, 2.0f
        );
    }

    d->AddText(nullptr, 18.0f, { center.x - 55, center.y - 40 }, pBright, "OSC-V1945");
    d->AddText(nullptr, 14.0f, { center.x - 70, center.y + radius - 50 }, pBright, "SIGNAL STRENGTH: 100%");

    for (int i = 0; i < (int)radius * 2; i += 5) {
        float lineY = (center.y - radius) + i;
        if (lineY > center.y - radius && lineY < center.y + radius)
            d->AddLine({center.x - radius, lineY}, {center.x + radius, lineY}, ImColor(0, 0, 0, 50), 1.0f);
    }
}