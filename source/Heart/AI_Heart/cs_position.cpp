#include "cs_position.h"
#include "raylib.h"

void SetAiUiPosition(UiPos& pos) {
    pos.width = 250.0f;
    pos.padding = 10.0f;
    
    pos.x = (float)GetScreenWidth() - pos.width - pos.padding;
    pos.y = pos.padding;

    ImGui::SetNextWindowPos({ pos.x, pos.y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ pos.width, 0 }, ImGuiCond_Once);
}