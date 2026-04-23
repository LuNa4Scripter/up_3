#include "cs_player_setting_UI.h"
#include "cs_click.h"
#include "../../Animation/UI_Animation/Player_setting/cs_player_setting_animation.h"
#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h"
#include "../../data/cs_data.h"
#include <math.h>

void DrawPlayerSettingUI() {
    if (!isSettingsOpen && settingAnimPercent <= 0.0f) return;

    static int currentTab = 0;
    static float timer = 0.0f;
    timer += GetFrameTime();

    ImDrawList* d = ImGui::GetForegroundDrawList();
    float ease = GetEaseOutBack(settingAnimPercent);
    
    ImVec2 uiSize = { 550.0f, 700.0f };
    float x = (100.0f * ease) - 40.0f;
    float y = (SCREEN_HEIGHT - uiSize.y) / 2.0f;
    ImVec2 m = ImGui::GetMousePos();

    const char* tabs[] = { "SYSTEM_CONFIG", "AUDIO_MATRIX", "NETWORK_LINK", "TERMINATE" };
    int tabCount = 4;

    if (ImGui::IsKeyPressed(ImGuiKey_W) || ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        currentTab = (currentTab - 1 + tabCount) % tabCount;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_S) || ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        currentTab = (currentTab + 1) % tabCount;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        if (currentTab == 3) {
            isSettingsOpen = false;
            isGamePaused = false;
        }
    }

    ImColor mainClr = ImColor(0, 255, 150, (int)(255 * settingAnimPercent));
    ImColor bgClr = ImColor(5, 10, 5, (int)(240 * settingAnimPercent));
    ImColor dimClr = ImColor(0, 255, 150, (int)(80 * settingAnimPercent));

    d->AddRectFilled({x, y}, {x + uiSize.x, y + uiSize.y}, bgClr);
    d->AddRect({x, y}, {x + uiSize.x, y + uiSize.y}, mainClr, 0, 0, 1.5f);

    d->AddRectFilled({x, y}, {x + uiSize.x, y + 40}, dimClr);
    d->AddText(nullptr, 18.0f, {x + 20, y + 12}, ImColor(0, 0, 0), "SYS_CORE_INTERFACE // AUTH_LEVEL: ROOT");

    for (int i = 0; i < tabCount; i++) {
        ImVec2 bPos = {x + 30, y + 80 + (i * 70)};
        ImVec2 bSize = {uiSize.x - 60, 55};
        
        bool mouseHover = (m.x > bPos.x && m.x < bPos.x + bSize.x && m.y > bPos.y && m.y < bPos.y + bSize.y);
        bool isSelected = (currentTab == i);

        if (mouseHover && ImGui::IsMouseClicked(0)) {
            currentTab = i;
            if (i == 3) {
                isSettingsOpen = false;
                isGamePaused = false;
            }
        }

        ImColor bCol = isSelected ? mainClr : (mouseHover ? ImColor(0, 255, 150, 40) : ImColor(0, 0, 0, 0));
        d->AddRectFilled(bPos, {bPos.x + bSize.x, bPos.y + bSize.y}, bCol, 2.0f);
        d->AddRect(bPos, {bPos.x + bSize.x, bPos.y + bSize.y}, mainClr, 2.0f, 0, 1.0f);

        if (isSelected) {
            float pulse = fabsf(sinf(timer * 4.0f)) * 30;
            d->AddRectFilled({bPos.x - 10, bPos.y}, {bPos.x - 5, bPos.y + bSize.y}, mainClr);
            d->AddRect({bPos.x, bPos.y}, {bPos.x + bSize.x, bPos.y + bSize.y}, ImColor(255, 255, 255, (int)pulse), 2.0f);
        }

        ImColor textCol = isSelected ? ImColor(0, 0, 0) : mainClr;
        d->AddText(nullptr, 22.0f, {bPos.x + 20, bPos.y + 15}, textCol, tabs[i]);
        
        if (!isSelected) {
            d->AddText(nullptr, 14.0f, {bPos.x + bSize.x - 80, bPos.y + 20}, dimClr, TextFormat("0x0%d", i));
        }
    }

    float infoY = y + 400;
    d->AddLine({x + 30, infoY}, {x + uiSize.x - 30, infoY}, dimClr, 1.0f);
    
    const char* details[] = { "LOG: RECV_DATA_SUCCESS", "VOL: 85%_STABLE", "NET: 127.0.0.1", "SES: ACTIVE" };
    d->AddText(nullptr, 15.0f, {x + 35, infoY + 20}, mainClr, details[currentTab]);

    for(int j=0; j<15; j++) {
        float rY = infoY + 60 + (j * 15);
        float a = (sinf(timer * 5.0f + j) * 0.5f + 0.5f) * 0.3f;
        d->AddText(nullptr, 12.0f, {x + 35, rY}, ImColor(0, 255, 150, (int)(255 * a)), 
            TextFormat("0x%04X: %02X %02X %02X %02X", 0x1A00 + j, rand()%0xFF, rand()%0xFF, rand()%0xFF, rand()%0xFF));
    }

    float footerY = y + uiSize.y - 40;
    d->AddText(nullptr, 13.0f, {x + 30, footerY}, dimClr, "[W/S]: NAVIGATE  [ENTER]: SELECT  [MOUSE]: POINT");
    
    float scanPos = fmodf(timer * 200.0f, uiSize.y);
    d->AddLine({x, y + scanPos}, {x + uiSize.x, y + scanPos}, ImColor(0, 255, 150, 15));
}