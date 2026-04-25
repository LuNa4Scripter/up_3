#include "cs_decoration.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h"
#include <vector>
#include <string>
#include <math.h>

#define C_REAL_METAL      ImColor(38, 40, 42, 255)
#define C_WEAR_EDGE       ImColor(120, 122, 125, 100)
#define C_GAUGE_FACE      ImColor(15, 16, 17, 255)
#define C_LUM_OFF         ImColor(140, 160, 130, 180)
#define C_NEEDLE          ImColor(180, 40, 30, 255)
#define C_SKY_NATURAL     ImColor(165, 185, 210, 255)
#define C_BELT_FABRIC     ImColor(130, 115, 90, 255)
#define C_WIRE_BROWN      ImColor(90, 75, 60, 255)
#define C_RIVET_SHADOW    ImColor(10, 10, 10, 150)
#define C_AGED_LEATHER    ImColor(85, 55, 40, 255)

static float cockpitTimer = 0.0f;
static Camera3D mainCam = { 0 };
static Vector3 shakeP = { 0, 0, 0 };
static float impactE = 0.0f;

void InitDiegeticUI() {
    cockpitTimer = 0.0f;
    mainCam.position = (Vector3){ 0.0f, 1.6f, 0.8f };
    mainCam.target = (Vector3){ 0.0f, 1.58f, 10.0f };
    mainCam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    mainCam.fovy = 65.0f;
    mainCam.projection = CAMERA_PERSPECTIVE;
}

void TriggerImpact(float intensity) {
    impactE = intensity;
}

void UpdateDiegeticUI(float dt) {
    cockpitTimer += dt;
    float vH = sinf(cockpitTimer * 115.0f) * 0.0012f;
    float vL = sinf(cockpitTimer * 0.8f) * 0.04f;
    float iX = sinf(cockpitTimer * 55.0f) * impactE;
    impactE = Lerp(impactE, 0.0f, dt * 7.0f);
    shakeP.x = vH + vL + iX;
    shakeP.y = vH + (cosf(cockpitTimer * 0.7f) * 0.03f) + (impactE * 0.5f);
    mainCam.position.x = shakeP.x;
    mainCam.position.y = 1.6f + shakeP.y;
    mainCam.target.x = shakeP.x * 0.4f;
    mainCam.target.y = 1.58f + shakeP.y * 0.4f;
}

void DrawDiegeticUI() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float ux = shakeP.x * 400.0f;
    float uy = shakeP.y * 400.0f;
    dl->AddRectFilled({0, 0}, {sw, sh}, C_SKY_NATURAL);
    dl->AddCircleFilled({sw * 0.75f, sh * 0.25f}, 120.0f, ImColor(255, 252, 240, 150));
    ImVec2 canopy[6] = {
        {sw * 0.33f + ux, -60}, {sw * 0.67f + ux, -60},
        {sw * 0.76f + ux, sh * 0.45f}, {sw * 0.96f + ux, sh + 50},
        {sw * 0.04f + ux, sh + 50}, {sw * 0.24f + ux, sh * 0.45f}
    };
    dl->AddConvexPolyFilled(canopy, 6, C_REAL_METAL);
    dl->AddPolyline(canopy, 6, C_WEAR_EDGE, ImDrawFlags_Closed, 1.2f);
    for (int i = 0; i < 10; i++) {
        dl->AddCircleFilled({canopy[2].x - 10, canopy[2].y + i * 50}, 3.0f, C_RIVET_SHADOW);
        dl->AddCircleFilled({canopy[2].x - 11, canopy[2].y + i * 50 - 1}, 1.5f, C_WEAR_EDGE);
        dl->AddCircleFilled({canopy[5].x + 10, canopy[5].y + i * 50}, 3.0f, C_RIVET_SHADOW);
        dl->AddCircleFilled({canopy[5].x + 9, canopy[5].y + i * 50 - 1}, 1.5f, C_WEAR_EDGE);
    }
    dl->AddBezierQuadratic({sw * 0.26f + ux, sh * 0.47f}, {sw * 0.5f + ux, sh * 0.37f}, {sw * 0.74f + ux, sh * 0.47f}, ImColor(70, 72, 75, 255), 20.0f);
    ImVec2 mP = {sw * 0.15f + ux * 1.05f, sh * 0.45f + uy * 1.05f};
    ImVec2 mS = {sw * 0.7f, sh * 0.8f};
    dl->AddRectFilled(mP, {mP.x + mS.x, mP.y + mS.y}, ImColor(45, 47, 48, 255), 4.0f);
    dl->AddRectFilledMultiColor(mP, {mP.x + mS.x, mP.y + 40}, ImColor(255, 255, 255, 20), ImColor(255, 255, 255, 10), ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0));
    for (int i = 0; i < 12; i++) {
        float gx = mP.x + 100 + (i % 6 * 140);
        float gy = mP.y + 100 + (i / 6 * 140);
        dl->AddCircleFilled({gx, gy}, 62.0f, C_GAUGE_FACE);
        dl->AddCircle({gx, gy}, 63.0f, C_WEAR_EDGE, 32, 1.0f);
        for (int j = 0; j < 12; j++) {
            float tickA = j * (6.28f / 12.0f);
            dl->AddLine({gx + cosf(tickA) * 50, gy + sinf(tickA) * 50}, {gx + cosf(tickA) * 58, gy + sinf(tickA) * 58}, ImColor(200, 200, 200, 100), 1.0f);
        }
        float nA = -1.4f + sinf(cockpitTimer * (0.3f + i * 0.12f)) * 0.7f;
        dl->AddLine({gx, gy}, {gx + cosf(nA) * 45, gy + sinf(nA) * 45}, C_NEEDLE, 2.2f);
        dl->AddCircleFilled({gx, gy}, 4.0f, ImColor(30, 30, 30, 255));
    }
    dl->AddRectFilled({0, sh * 0.68f + uy}, {sw * 0.22f + ux, sh}, ImColor(40, 42, 44, 255));
    dl->AddRectFilled({sw * 0.78f + ux, sh * 0.68f + uy}, {sw, sh}, ImColor(40, 42, 44, 255));
    for (int k = 0; k < 5; k++) {
        float swX = sw * 0.05f + ux + k * 40;
        float swY = sh * 0.85f + uy;
        dl->AddRectFilled({swX, swY}, {swX + 15, swY + 30}, ImColor(20, 21, 22, 255));
        dl->AddRectFilled({swX + 5, swY + 5}, {swX + 10, swY + 25}, C_WEAR_EDGE);
    }
    float stX = sw * 0.5f + ux * 0.7f;
    float stY = sh * 0.88f + uy * 0.7f;
    dl->AddRectFilled({stX - 18, stY - 160}, {stX + 18, stY + 200}, C_REAL_METAL, 4.0f);
    dl->AddCircleFilled({stX, stY + 80}, 85.0f, C_AGED_LEATHER);
    dl->AddCircleFilled({stX, stY - 160}, 32.0f, ImColor(30, 31, 32, 255));
    dl->AddRectFilled({stX - 5, stY - 100}, {stX + 5, stY - 40}, ImColor(180, 50, 40, 255));
    ImVec2 bL[4] = {{sw * 0.38f + ux, sh}, {sw * 0.43f + ux, sh * 0.78f}, {sw * 0.47f + ux, sh * 0.78f}, {sw * 0.44f + ux, sh}};
    ImVec2 bR[4] = {{sw * 0.62f + ux, sh}, {sw * 0.57f + ux, sh * 0.78f}, {sw * 0.53f + ux, sh * 0.78f}, {sw * 0.56f + ux, sh}};
    dl->AddConvexPolyFilled(bL, 4, C_BELT_FABRIC);
    dl->AddConvexPolyFilled(bR, 4, C_BELT_FABRIC);
    dl->AddPolyline(bL, 4, ImColor(0, 0, 0, 80), true, 1.5f);
    dl->AddPolyline(bR, 4, ImColor(0, 0, 0, 80), true, 1.5f);
    dl->AddBezierCubic({sw * 0.2f + ux, sh * 0.7f + uy}, {sw * 0.25f + ux, sh * 0.85f + uy}, {sw * 0.3f + ux, sh * 0.65f + uy}, {sw * 0.35f + ux, sh * 0.75f + uy}, C_WIRE_BROWN, 2.0f);
    dl->AddRectFilledMultiColor({0, 0}, {sw, sh}, ImColor(255, 255, 255, 8), ImColor(255, 255, 255, 2), ImColor(0, 0, 0, 25), ImColor(0, 0, 0, 15));
    dl->AddRectFilled({0, 0}, {sw, sh}, ImColor(255, 255, 255, 5));
}

void DrawExtraBolts(ImDrawList* dl, float sw, float sh, float ux, float uy) {
    for (int i = 0; i < 20; i++) {
        float bx = (i < 10) ? sw * 0.12f : sw * 0.88f;
        float by = sh * 0.4f + (i % 10) * 60;
        dl->AddCircleFilled({bx + ux, by + uy}, 4.0f, C_RIVET_SHADOW);
        dl->AddCircleFilled({bx + ux - 1, by + uy - 1}, 2.0f, C_WEAR_EDGE);
    }
}

void DrawFloorDetail(ImDrawList* dl, float sw, float sh, float ux, float uy) {
    dl->AddRectFilled({sw * 0.3f + ux, sh * 0.95f + uy}, {sw * 0.7f + ux, sh}, ImColor(25, 26, 27, 255));
    dl->AddLine({sw * 0.35f + ux, sh * 0.96f + uy}, {sw * 0.65f + ux, sh * 0.96f + uy}, C_WEAR_EDGE, 1.0f);
}

void RenderCockpitFinal() {
    float dt = GetFrameTime();
    UpdateDiegeticUI(dt);
    rlImGuiBegin();
    DrawDiegeticUI();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float ux = shakeP.x * 400.0f;
    float uy = shakeP.y * 400.0f;
    DrawExtraBolts(dl, sw, sh, ux, uy);
    DrawFloorDetail(dl, sw, sh, ux, uy);
    dl->AddRectFilledMultiColor({0, 0}, {sw, sh}, ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 60), ImColor(0, 0, 0, 60));
    rlImGuiEnd();
}

void CloseRealisticSim() {
    rlImGuiShutdown();
}

void Placeholder_1() { return; }
void Placeholder_2() { return; }
void Placeholder_3() { return; }
void Placeholder_4() { return; }
void Placeholder_5() { return; }
void Placeholder_6() { return; }
void Placeholder_7() { return; }
void Placeholder_8() { return; }
void Placeholder_9() { return; }
void Placeholder_10() { return; }
void Placeholder_11() { return; }
void Placeholder_12() { return; }
void Placeholder_13() { return; }
void Placeholder_14() { return; }
void Placeholder_15() { return; }
void Placeholder_16() { return; }
void Placeholder_17() { return; }
void Placeholder_18() { return; }
void Placeholder_19() { return; }
void Placeholder_20() { return; }
void Placeholder_21() { return; }
void Placeholder_22() { return; }
void Placeholder_23() { return; }
void Placeholder_24() { return; }
void Placeholder_25() { return; }
void Placeholder_26() { return; }
void Placeholder_27() { return; }
void Placeholder_28() { return; }
void Placeholder_29() { return; }
void Placeholder_30() { return; }
void Placeholder_31() { return; }
void Placeholder_32() { return; }
void Placeholder_33() { return; }
void Placeholder_34() { return; }
void Placeholder_35() { return; }
void Placeholder_36() { return; }
void Placeholder_37() { return; }
void Placeholder_38() { return; }
void Placeholder_39() { return; }
void Placeholder_40() { return; }
void Placeholder_41() { return; }
void Placeholder_42() { return; }
void Placeholder_43() { return; }
void Placeholder_44() { return; }
void Placeholder_45() { return; }
void Placeholder_46() { return; }
void Placeholder_47() { return; }
void Placeholder_48() { return; }
void Placeholder_49() { return; }
void Placeholder_50() { return; }