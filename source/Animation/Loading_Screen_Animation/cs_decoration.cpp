#include "cs_decoration.h"
#include "../../data/cs_data.h"
#include "cs_shaking.h"
#include "raylib.h"
#include <math.h>
#include <vector>
#include <stdio.h>

static float animTimer = 0.0f;
static float glitchOffset = 0.0f;

struct DataNode {
    ImVec2 pos;
    float value;
    float speed;
    int type;
};
static std::vector<DataNode> nodes;

void InitDecoration() {
    animTimer = 0.0f;
    nodes.clear();
    for (int i = 0; i < 80; i++) {
        nodes.push_back({
            {(float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(0, SCREEN_HEIGHT)},
            0.0f, (float)GetRandomValue(3, 15) / 10.0f, GetRandomValue(0, 3)
        });
    }
}

void UpdateDecoration(float dt) {
    animTimer += dt;
    glitchOffset = (fmodf(animTimer, 2.7f) < 0.15f) ? (float)GetRandomValue(-25, 25) : 0.0f;
    for (auto& n : nodes) {
        n.pos.y -= n.speed * 25.0f * dt;
        if (n.pos.y < -20) n.pos.y = SCREEN_HEIGHT + 20;
        n.value = sinf(animTimer * n.speed);
    }
}

void DrawDecoration() {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 shake = GetImGuiShakeOffset();
    float gx = glitchOffset;
    ImVec2 center = ImVec2(SCREEN_WIDTH / 2.0f + shake.x, SCREEN_HEIGHT / 2.0f + shake.y);

    ImU32 cLow = ImColor(0, 255, 245, 12);
    ImU32 cMid = ImColor(0, 255, 245, 45);
    ImU32 cHigh = ImColor(0, 255, 245, 130);

    for (int i = 0; i <= SCREEN_WIDTH; i += 40) {
        drawList->AddLine(ImVec2(i + shake.x + gx, 0), ImVec2(i + shake.x - gx, SCREEN_HEIGHT), cLow, 1.0f);
    }
    for (int i = 0; i <= SCREEN_HEIGHT; i += 40) {
        drawList->AddLine(ImVec2(0, i + shake.y), ImVec2(SCREEN_WIDTH, i + shake.y), cLow, 1.0f);
    }

    for (int i = 0; i < 6; i++) {
        float r = 100.0f + (i * 60.0f) + sinf(animTimer * 0.8f + i) * 15.0f;
        float rot = animTimer * (i % 2 == 0 ? 0.5f : -0.7f);
        drawList->AddCircle(center, r, ImColor(0, 255, 245, 25 - i * 3), 120, 1.0f);
        
        for (int j = 0; j < 8; j++) {
            float ang = rot + (j * 0.785f);
            ImVec2 p1 = { center.x + cosf(ang) * (r - 15), center.y + sinf(ang) * (r - 15) };
            ImVec2 p2 = { center.x + cosf(ang) * (r + 15), center.y + sinf(ang) * (r + 15) };
            drawList->AddLine(p1, p2, cMid, 1.5f);
            
            if (i == 2) {
                drawList->AddCircleFilled(p2, 3.0f, cHigh);
            }
        }
    }

    for (int i = 0; i < 360; i += 5) {
        float rad = i * 0.0174f;
        float r1 = 280.0f + sinf(animTimer * 2.0f + i * 0.1f) * 10.0f;
        ImVec2 p = { center.x + cosf(rad) * r1, center.y + sinf(rad) * r1 };
        drawList->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 2, p.y + 2), ImColor(0, 255, 245, 30));
    }

    for (auto& n : nodes) {
        ImVec2 p = ImVec2(n.pos.x + shake.x + gx, n.pos.y + shake.y);
        float a = (n.value * 0.5f + 0.5f);
        drawList->AddRectFilled(p, ImVec2(p.x + 2, p.y + 2), ImColor(0, 255, 245, (int)(180 * a)));
        if (n.type == 1 && a > 0.9f) {
            drawList->AddLine(p, center, ImColor(0, 255, 245, 10), 0.5f);
        }
    }

    for (int i = 0; i < 15; i++) {
        float bh = 15.0f + sinf(animTimer * 5.0f + i) * 20.0f;
        ImVec2 bp = ImVec2(center.x - 180 + (i * 25), center.y + 250);
        drawList->AddRectFilled(bp, ImVec2(bp.x + 18, bp.y + bh), cMid);
        drawList->AddRect(bp, ImVec2(bp.x + 18, bp.y + 50), cLow);
    }

    float sy = fmodf(animTimer * 600.0f, SCREEN_HEIGHT * 1.5f) - 300.0f;
    drawList->AddRectFilledMultiColor(ImVec2(0, sy), ImVec2(SCREEN_WIDTH, sy + 120),
        ImColor(0, 255, 245, 0), ImColor(0, 255, 245, 0), cLow, cLow);
    drawList->AddLine(ImVec2(0 + gx, sy + 120), ImVec2(SCREEN_WIDTH + gx, sy + 120), cMid, 1.5f);

    for (int i = 0; i < 40; i++) {
        float x = SCREEN_WIDTH - 220.0f;
        float y = 80.0f + (i * 14);
        if (y > SCREEN_HEIGHT - 80) break;
        float v = sinf(animTimer * 4.0f + i) * 0.5f + 0.5f;
        char db[16]; sprintf(db, "DATA_%02d", i);
        drawList->AddText(ImVec2(x + shake.x, y + shake.y), ImColor(0, 255, 245, (int)(120 * v)), db);
        drawList->AddRectFilled(ImVec2(x + 70 + shake.x, y + 5 + shake.y), ImVec2(x + 70 + (v * 100) + shake.x, y + 10 + shake.y), cMid);
    }

    float m = 30.0f;
    ImVec2 cn[4] = {{m, m}, {SCREEN_WIDTH-m, m}, {SCREEN_WIDTH-m, SCREEN_HEIGHT-m}, {m, SCREEN_HEIGHT-m}};
    for(int i=0; i<4; i++) {
        drawList->AddCircle(cn[i], 15.0f, cHigh, 6, 2.5f);
        drawList->AddLine(cn[i], ImVec2(cn[i].x + (i%2==0?100:-100), cn[i].y), cMid, 1.5f);
        drawList->AddLine(cn[i], ImVec2(cn[i].x, cn[i].y + (i<2?100:-100)), cMid, 1.5f);
    }

    for (int i = 0; i < 6; i++) {
        char st[32];
        const char* l[] = {"BUFFER", "SIGNAL", "KERNEL", "STREAM", "UPLINK", "RENDER"};
        sprintf(st, "[%s] >> %02d%%", l[i], (int)(fmodf(animTimer * (20 + i), 100.0f)));
        drawList->AddText(ImVec2(60 + shake.x + gx, 60 + (i * 20) + shake.y), cHigh, st);
    }
}