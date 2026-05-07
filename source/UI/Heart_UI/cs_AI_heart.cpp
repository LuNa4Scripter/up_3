#include "cs_AI_heart.h"
#include <string>

void DrawAiHeartUI(EnemyStatus* units, int count) {
    ImGui::SetNextWindowSize({250, 350}, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("AI_CORE_MONITOR")) {
        ImGui::End();
        return;
    }

    for (int i = 0; i < count; i++) {
        if (!units[i].isActive) continue;

        std::string idTag = "ID: " + std::to_string(units[i].unitId);
        float hpRatio = units[i].currentHp / units[i].maxHp;

        ImGui::Text("%s", idTag.c_str());
        
        ImVec4 hpColor = ImVec4(0.0f, 0.9f, 0.9f, 1.0f);
        if (hpRatio < 0.25f) {
            hpColor = ImVec4(1.0f, 0.0f, 0.3f, 1.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, hpColor);
        
        std::string hpText = std::to_string((int)units[i].currentHp) + "/" + std::to_string((int)units[i].maxHp);
        ImGui::ProgressBar(hpRatio, ImVec2(-1.0f, 14.0f), hpText.c_str());
        
        ImGui::PopStyleColor();
        ImGui::Separator();
    }

    ImGui::End();
}