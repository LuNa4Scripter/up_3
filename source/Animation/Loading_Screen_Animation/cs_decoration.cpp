#include "cs_decoration.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <math.h>

#define C_BASE            ImColor(10, 12, 14, 255)
#define C_SPEC            ImColor(90, 95, 105, 255)
#define C_HUD_G           ImColor(0, 255, 100, 240)
#define C_HUD_SCAN        ImColor(0, 255, 100, 40)
#define C_SKY_U           ImColor(2, 4, 20, 255)
#define C_SKY_L           ImColor(140, 180, 255, 255)
#define C_SUN             ImColor(255, 255, 220, 255)
#define C_WARN            ImColor(255, 10, 0, 255)
#define C_STITCH          ImColor(0, 0, 0, 180)
#define C_GLARE           ImColor(255, 255, 255, 25)
#define C_METAL           ImColor(40, 42, 45, 255)

static float ct = 0.0f;
static Vector3 sp = { 0, 0, 0 };
static float ie = 0.0f;
static float transitionAlpha = 0.0f;

void InitDecoration() {
    ct = 0.0f;
    ie = 0.0f;
    transitionAlpha = 0.0f;
}

void TriggerImpact(float v) {
    ie = v;
}

void UpdateDecoration(float dt) {
    ct += dt;
    float h = sinf(ct * 210.0f) * 0.0015f;
    float l = sinf(ct * 0.4f) * 0.05f;
    float x = sinf(ct * 95.0f) * ie;
    ie = Lerp(ie, 0.0f, dt * 6.0f);
    sp.x = h + l + x;
    sp.y = h + (cosf(ct * 0.5f) * 0.03f) + (ie * 0.6f);
    
    if (transitionAlpha < 1.0f) {
        transitionAlpha += dt * 1.5f;
        if (transitionAlpha > 1.0f) transitionAlpha = 1.0f;
    }
}

void DrawEnvironment(ImDrawList* dl, float sw, float sh, float ux, float uy) {
    dl->AddRectFilledMultiColor(ImVec2(-200, -200), ImVec2(sw + 200, sh + 200), C_SKY_U, C_SKY_U, C_SKY_L, C_SKY_L);
    for (int i = 0; i < 18; i++) {
        float z = fmodf(ct * 320.0f + i * 450.0f, 3000.0f);
        float s = 1.0f - (z / 3000.0f);
        if (s < 0) s = 0;
        float cx = sw * 0.5f + (sinf(i * 2.1f) * sw * 1.5f) + ux * s;
        float cy = sh * 0.3f + (cosf(i * 1.2f) * 250.0f) + uy * s;
        dl->AddEllipseFilled(ImVec2(cx, cy), ImVec2(900.0f * s, 180.0f * s), ImColor(255, 255, 255, (int)(55 * s)));
    }
    float sx = sw * 0.85f + ux * 0.1f;
    float sy = sh * 0.15f + uy * 0.1f;
    for(int i = 0; i < 12; i++) {
        dl->AddCircleFilled(ImVec2(sx, sy), 40.0f + i * 80.0f, ImColor(255, 255, 200, 50 - i * 4));
    }
}

void DrawSeat(ImDrawList* dl, float sw, float sh, float ux, float uy) {
    float bx = sw * 0.5f + ux * 0.3f;
    float by = sh + 200 + uy * 0.3f;
    ImVec2 sb[] = { ImVec2(bx - 260, by), ImVec2(bx - 190, by - 720), ImVec2(bx + 190, by - 720), ImVec2(bx + 260, by) };
    dl->AddConvexPolyFilled(sb, 4, ImColor(20, 18, 16, 255));
    dl->AddPolyline(sb, 4, C_SPEC, true, 3.0f);
    dl->AddRectFilledMultiColor(ImVec2(bx - 180, by - 700), ImVec2(bx + 180, by - 150), ImColor(30, 25, 22, 255), ImColor(30, 25, 22, 255), ImColor(60, 50, 40, 255), ImColor(60, 50, 40, 255));
    for(int i = 0; i < 25; i++) {
        float y = by - 680 + i * 30;
        dl->AddLine(ImVec2(bx - 160, y), ImVec2(bx + 160, y), C_STITCH, 1.5f);
    }
    ImVec2 hr[] = { ImVec2(bx - 110, by - 720), ImVec2(bx - 125, by - 850), ImVec2(bx + 125, by - 850), ImVec2(bx + 110, by - 720) };
    dl->AddConvexPolyFilled(hr, 4, ImColor(30, 30, 32, 255));
    dl->AddRectFilled(ImVec2(bx - 60, by - 830), ImVec2(bx + 60, by - 790), C_WARN, 8.0f);
    dl->AddText(NULL, 22.0f, ImVec2(bx - 45, by - 825), ImColor(255, 255, 255), "ARMED");
}

void DrawMFD(ImDrawList* dl, ImVec2 p, ImVec2 s, int id) {
    dl->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(2, 8, 2, 255), 10.0f);
    dl->AddRect(p, ImVec2(p.x + s.x, p.y + s.y), C_SPEC, 10.0f, 0, 2.5f);
    for(int i = 0; i < (int)s.y; i += 3) dl->AddLine(ImVec2(p.x, p.y + i), ImVec2(p.x + s.x, p.y + i), ImColor(0, 20, 0, 80));
    float t = ct * 3.0f;
    if (id == 1) {
        ImVec2 c = ImVec2(p.x + s.x * 0.5f, p.y + s.y * 0.55f);
        dl->AddCircle(c, s.y * 0.35f, C_HUD_G, 32, 2.0f);
        dl->AddLine(c, ImVec2(c.x + cosf(t) * s.y * 0.35f, c.y + sinf(t) * s.y * 0.35f), C_HUD_G, 3.0f);
        dl->AddText(NULL, 20.0f, ImVec2(p.x + 20, p.y + 20), C_HUD_G, "RADAR: SCAN");
    } else {
        for(int k = 0; k < 8; k++) {
            float v = (sinf(ct * 6.0f + k * 0.5f) + 1.1f) * 40.0f;
            dl->AddRectFilled(ImVec2(p.x + 30 + k * 28, p.y + s.y - 30 - v), ImVec2(p.x + 52 + k * 28, p.y + s.y - 30), C_HUD_G);
        }
        dl->AddText(NULL, 20.0f, ImVec2(p.x + 20, p.y + 20), C_HUD_G, id == 0 ? "SYS: NORM" : "ENG: MAX");
    }
}

void DrawCockpit(ImDrawList* dl, float sw, float sh, float ux, float uy) {
    ImVec2 cp[] = { ImVec2(sw * 0.05f + ux, -300), ImVec2(sw * 0.95f + ux, -300), ImVec2(sw * 0.8f + ux, 180), ImVec2(sw * 0.2f + ux, 180) };
    dl->AddConvexPolyFilled(cp, 4, C_BASE);
    ImVec2 sl[] = { cp[0], cp[3], ImVec2(-100, sh), ImVec2(-300, sh) };
    ImVec2 sr[] = { cp[1], cp[2], ImVec2(sw + 100, sh), ImVec2(sw + 300, sh) };
    dl->AddConvexPolyFilled(sl, 4, C_BASE);
    dl->AddConvexPolyFilled(sr, 4, C_BASE);
    dl->AddPolyline(sl, 4, C_SPEC, true, 4.0f);
    dl->AddPolyline(sr, 4, C_SPEC, true, 4.0f);
    for(int i = 0; i < 15; i++) {
        dl->AddCircleFilled(ImVec2(sl[1].x - 20, sl[1].y + i * 75), 6.0f, ImColor(0, 0, 0, 255));
        dl->AddCircleFilled(ImVec2(sl[1].x - 22, sl[1].y + i * 75 - 3), 3.0f, ImColor(130, 135, 140, 220));
        dl->AddCircleFilled(ImVec2(sr[1].x + 20, sr[1].y + i * 75), 6.0f, ImColor(0, 0, 0, 255));
        dl->AddCircleFilled(ImVec2(sr[1].x + 18, sr[1].y + i * 75 - 3), 3.0f, ImColor(130, 135, 140, 220));
    }
    ImVec2 dp = ImVec2(sw * 0.02f + ux, sh * 0.4f + uy);
    ImVec2 ds = ImVec2(sw * 0.96f, sh * 0.7f);
    dl->AddRectFilled(dp, ImVec2(dp.x + ds.x, dp.y + ds.y), ImColor(18, 20, 22, 255), 30.0f);
    dl->AddRect(dp, ImVec2(dp.x + ds.x, dp.y + ds.y), C_SPEC, 30.0f, 0, 2.0f);
    for (int i = 0; i < 3; i++) {
        float w = ds.x / 3.0f;
        DrawMFD(dl, ImVec2(dp.x + 40 + i * w, dp.y + 70), ImVec2(w * 0.85f, ds.y * 0.55f), i);
    }
    for(int i = 0; i < 15; i++) {
        float x = dp.x + 120 + i * 130;
        float y = dp.y + ds.y - 120;
        if (x > dp.x + ds.x - 80) continue;
        dl->AddCircleFilled(ImVec2(x, y), 55.0f, ImColor(5, 6, 7, 255));
        dl->AddCircle(ImVec2(x, y), 56.0f, C_SPEC, 36, 2.0f);
        float a = -1.57f + sinf(ct * 1.1f + i) * 1.4f;
        dl->AddLine(ImVec2(x, y), ImVec2(x + cosf(a) * 45, y + sinf(a) * 45), C_WARN, 2.5f);
    }
}

void DrawStick(ImDrawList* dl, float sw, float sh, float ux, float uy) {
    float x = sw * 0.5f + ux * 0.8f;
    float y = sh * 0.95f + uy * 0.8f;
    ImVec2 sk[] = { ImVec2(x - 90, sh + 100), ImVec2(x - 50, y - 220), ImVec2(x + 50, y - 220), ImVec2(x + 90, sh + 100) };
    dl->AddConvexPolyFilled(sk, 4, ImColor(8, 9, 10, 255));
    dl->AddPolyline(sk, 4, C_SPEC, true, 3.5f);
    dl->AddCircleFilled(ImVec2(x, y - 240), 65.0f, ImColor(22, 24, 26, 255));
    dl->AddCircle(ImVec2(x, y - 240), 66.0f, C_SPEC, 40, 2.5f);
    dl->AddCircleFilled(ImVec2(x + 25, y - 260), 15.0f, C_WARN);
    dl->AddCircleFilled(ImVec2(x - 20, y - 275), 10.0f, ImColor(50, 50, 55, 255));
}

void DrawDecoration() {
    float dt = GetFrameTime();
    UpdateDecoration(dt);
    rlImGuiBegin();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
    float ux = sp.x * 900.0f, uy = sp.y * 900.0f;
    DrawEnvironment(dl, sw, sh, ux, uy);
    DrawSeat(dl, sw, sh, ux, uy);
    DrawCockpit(dl, sw, sh, ux, uy);
    DrawStick(dl, sw, sh, ux, uy);
    dl->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(sw, sh), C_GLARE, ImColor(0,0,0,0), ImColor(0,0,0,110), C_GLARE);
    for(int i = 0; i < 40; i++) if(GetRandomValue(0, 100) > 96) dl->AddCircleFilled(ImVec2((float)GetRandomValue(0, (int)sw), (float)GetRandomValue(0, (int)sh)), 1.5f, ImColor(255, 255, 255, 80));
    
    if (transitionAlpha < 1.0f) {
        dl->AddRectFilled({0, 0}, {sw, sh}, ImColor(20, 18, 15, (int)(255 * (1.0f - transitionAlpha))));
    }
    
    rlImGuiEnd();
}

void RenderCockpitFinal() { DrawDecoration(); }
void CloseRealisticSim() { rlImGuiShutdown(); }
void P1() {} void P2() {} void P3() {} void P4() {} void P5() {}
void P6() {} void P7() {} void P8() {} void P9() {} void P10() {}