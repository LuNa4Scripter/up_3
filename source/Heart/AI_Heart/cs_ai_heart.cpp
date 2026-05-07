#include "cs_ai_heart.h"
#include <string>

void DrawAiHeartUI(std::vector<Object>& units) {
    ImGui::SetNextWindowSize({ 220, 300 }, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("AI_MONITOR")) {
        ImGui::End();
        return;
    }

    for (int i = 0; i < (int)units.size(); i++) {
        if (!units[i].active) continue;

        std::string id = "UNIT_" + std::to_string(i);
        float hpPer = (float)units[i].hp / 100.0f; 

        ImGui::Text("%s", id.c_str());
        
        ImVec4 col = ImVec4(0.0f, 0.8f, 0.8f, 1.0f);
        if (hpPer < 0.3f) col = ImVec4(1.0f, 0.1f, 0.2f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
        ImGui::ProgressBar(hpPer, ImVec2(-1, 14));
        ImGui::PopStyleColor();
        ImGui::Separator();
    }
    ImGui::End();
}