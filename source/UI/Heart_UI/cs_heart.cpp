#include "cs_heart.h"
#include "../../include/imgui.h"
#include "../../include/imgui_internal.h"
#include <cmath>

void DrawPlayerHeartImgui(float currentHp, float maxHp) {
    float t = (float)ImGui::GetTime();
    ImVec2 dSize = ImGui::GetIO().DisplaySize;
    
    float wWidth = 240.0f;
    float wHeight = 65.0f;
    ImVec2 p = ImVec2(40, dSize.y - wHeight - 30); 

    ImGui::SetNextWindowPos(p);
    ImGui::SetNextWindowSize(ImVec2(wWidth + 60, wHeight + 20));
    
    ImGuiWindowFlags f = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("NEURAL_LINK_OS", nullptr, f)) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 base = ImGui::GetCursorScreenPos();
        
        float hpPct = ImClamp(currentHp / maxHp, 0.0f, 1.0f);
        
        ImU32 c_accent = (hpPct > 0.3f) ? IM_COL32(0, 200, 255, 255) : IM_COL32(255, 50, 50, 255);
        ImU32 c_bg = IM_COL32(5, 10, 15, 180);

        ImVec2 pts[4];
        pts[0] = ImVec2(base.x + 15, base.y);
        pts[1] = ImVec2(base.x + wWidth + 15, base.y);
        pts[2] = ImVec2(base.x + wWidth, base.y + wHeight);
        pts[3] = ImVec2(base.x, base.y + wHeight);
        dl->AddConvexPolyFilled(pts, 4, c_bg);

        float sideSize = 10.0f;
        
        ImVec2 l_pts[3];
        l_pts[0] = ImVec2(base.x + 15 + sideSize, base.y);
        l_pts[1] = ImVec2(base.x + 15, base.y);
        l_pts[2] = ImVec2(base.x, base.y + wHeight);
        dl->AddPolyline(l_pts, 3, c_accent, 0, 1.5f);

        ImVec2 r_pts[3];
        r_pts[0] = ImVec2(base.x + wWidth + 15 - sideSize, base.y);
        r_pts[1] = ImVec2(base.x + wWidth + 15, base.y);
        r_pts[2] = ImVec2(base.x + wWidth, base.y + wHeight);
        dl->AddPolyline(r_pts, 3, c_accent, 0, 1.5f);

        ImGui::SetCursorScreenPos(ImVec2(base.x + 25, base.y + 4));
        ImGui::SetWindowFontScale(0.7f);
        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(c_accent), "LINK_STATUS: STABLE");

        ImGui::SetCursorScreenPos(ImVec2(base.x + 25, base.y + 18));
        ImGui::SetWindowFontScale(1.8f);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%02d", (int)currentHp);
        
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
        ImGui::SetWindowFontScale(0.85f);
        ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), "/ %02d", (int)maxHp);

        float barW = 140.0f;
        float barH = 10.0f;
        ImVec2 barP = ImVec2(base.x + 85, base.y + 36);
        
        dl->AddRectFilled(barP, ImVec2(barP.x + barW, barP.y + barH), IM_COL32(40, 40, 40, 100));
        
        float curBarW = barW * hpPct;
        if (curBarW > 0) {
            dl->AddRectFilled(barP, ImVec2(barP.x + curBarW, barP.y + barH), c_accent);
            
            for(int i = 0; i < 10; i++) {
                float segmentX = barP.x + (barW / 10.0f) * i;
                dl->AddLine(ImVec2(segmentX, barP.y), ImVec2(segmentX, barP.y + barH), IM_COL32(0, 0, 0, 150), 1.0f);
            }
        }

        float waveOffset = t * 5.0f;
        for (int i = 0; i < 15; i++) {
            float x1 = base.x + wWidth - 65 + (i * 4);
            float y1 = base.y + 20 + sinf(waveOffset + i * 0.6f) * 4.0f;
            float x2 = x1 + 4;
            float y2 = base.y + 20 + sinf(waveOffset + (i + 1) * 0.6f) * 4.0f;
            dl->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), c_accent, 1.0f);
        }

        ImGui::SetCursorScreenPos(ImVec2(base.x + wWidth - 60, base.y + 48));
        ImGui::SetWindowFontScale(0.65f);
        if (fmodf(t, 1.0f) > 0.2f) {
            ImGui::TextColored(ImVec4(1, 1, 1, 0.4f), "CORE_TEMP: 36.5C");
        }

        ImGui::SetCursorScreenPos(base);
        ImGui::Dummy(ImVec2(wWidth + 30, wHeight));

        ImGui::End();
    }
}