#include "cs_text_falling.h"
#include "../../data/cs_data.h"
#include "../../include/imgui.h"
#include "cs_shaking.h"
#include <math.h>
#include <vector>
#include <string>
#include <stdio.h>

struct CyberParticle {
    ImVec2 pos;
    ImVec2 vel;
    float life;
    float maxLife;
    ImU32 color;
    float size;
    float rotation;
    float rotVel;
    int type;
};

struct Shockwave {
    ImVec2 center;
    float radius;
    float maxRadius;
    float thickness;
    float life;
    ImU32 color;
};

struct SteamVapor {
    ImVec2 pos;
    ImVec2 vel;
    float life;
    float size;
    ImU32 color;
};

struct GearState {
    float rotation;
    float speed;
};

static float textPosY = -300.0f;
static float targetY = SCREEN_HEIGHT / 2.0f;
static float velocityY = 0.0f;
static float slashProgress = 0.0f;
static bool isFinished = false;
static bool hasShaked = false;
static float slashAlpha = 0.0f;
static float settleTimer = 0.0f;
static float internalTimer = 0.0f;
static float impactFlash = 0.0f;
static float bounceOffset = 0.0f;

static std::vector<CyberParticle> particles;
static std::vector<Shockwave> shockwaves;
static std::vector<SteamVapor> vapors;
static GearState leftGear = {0.0f, 120.0f};
static float pressureVal = 0.0f;

void InitTextFalling() {
    textPosY = -300.0f;
    velocityY = 0.0f;
    slashProgress = 0.0f;
    slashAlpha = 0.0f;
    settleTimer = 0.0f;
    internalTimer = 0.0f;
    impactFlash = 0.0f;
    bounceOffset = 0.0f;
    isFinished = false;
    hasShaked = false;
    particles.clear();
    shockwaves.clear();
    vapors.clear();
    leftGear.rotation = 0.0f;
}

void UpdateTextFalling(float dt) {
    internalTimer += dt;
    if (impactFlash > 0.0f) impactFlash -= dt * 3.5f;
    
    pressureVal = (textPosY / targetY);
    if (pressureVal > 1.0f) pressureVal = 1.0f;
    if (pressureVal < 0.0f) pressureVal = 0.0f;

    leftGear.rotation += dt * leftGear.speed * (1.0f + pressureVal * 3.0f);

    if (textPosY >= targetY || slashProgress > 0.0f) {
        if (rand() % 10 > 6) {
            SteamVapor v;
            v.pos = { (float)SCREEN_WIDTH / 2.0f + (rand() % 200 - 100), (float)targetY + (rand() % 100 - 50) };
            v.vel = { (float)(rand() % 400 - 200), (float)(rand() % -600 - 200) };
            v.life = 1.0f;
            v.size = (float)(rand() % 40 + 20);
            v.color = ImColor(220, 210, 190, 160);
            vapors.push_back(v);
        }
    }

    for (int i = 0; i < (int)vapors.size(); i++) {
        vapors[i].pos.x += vapors[i].vel.x * dt;
        vapors[i].pos.y += vapors[i].vel.y * dt;
        vapors[i].life -= dt * 1.5f;
        if (vapors[i].life <= 0) { vapors.erase(vapors.begin() + i); i--; }
    }

    if (textPosY < targetY) {
        float gravity = 15000.0f;
        float drag = 0.90f;
        velocityY += gravity * dt;
        velocityY *= (1.0f - (1.0f - drag) * dt * 60.0f);
        textPosY += velocityY * dt;

        if (textPosY >= targetY) {
            textPosY = targetY;
            impactFlash = 1.5f;
            bounceOffset = 60.0f;
            if (!hasShaked) {
                StartImGuiShake(0.7f, 150.0f);
                hasShaked = true;
            }
        }
    } else {
        if (bounceOffset > 0) bounceOffset -= dt * 250.0f;
        if (settleTimer < 0.5f) settleTimer += dt;
        else {
            if (slashProgress == 0.0f) {
                StartImGuiShake(1.0f, 250.0f);
                impactFlash = 1.0f;
            }
            slashProgress += dt;
            if (slashProgress < 0.25f) slashAlpha = 1.0f;
            else slashAlpha -= 1.8f * dt;
            if (slashProgress > 2.0f) isFinished = true;
        }
    }
}

void DrawTextFalling() {
    ImVec2 shake = GetImGuiShakeOffset();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT});
    ImGui::Begin("Industrial_Overlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddRectFilled({0, 0}, {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, ImColor(20, 18, 15, 255));
    
    for(int i = 0; i < SCREEN_HEIGHT; i += 2) {
        drawList->AddLine({0, (float)i}, {(float)SCREEN_WIDTH, (float)i}, ImColor(0, 0, 0, 60));
    }

    auto DrawHeavyGear = [&](ImVec2 pos, float radius, float rot) {
        for (int i = 0; i < 12; i++) {
            float a = (rot + i * 30.0f) * 0.0174f;
            drawList->AddLine({pos.x + cosf(a)*radius, pos.y + sinf(a)*radius}, {pos.x + cosf(a)*(radius+60), pos.y + sinf(a)*(radius+60)}, ImColor(100, 80, 50, 200), 45.0f);
        }
        drawList->AddCircleFilled(pos, radius, ImColor(45, 35, 25));
        drawList->AddCircle(pos, radius, ImColor(212, 175, 55, 120), 64, 10.0f);
    };

    DrawHeavyGear({-100 + shake.x, (float)SCREEN_HEIGHT + 100 + shake.y}, 400, leftGear.rotation);
    DrawHeavyGear({(float)SCREEN_WIDTH + 100 + shake.x, -100 + shake.y}, 350, -leftGear.rotation * 1.3f);

    for (auto& v : vapors) {
        drawList->AddCircleFilled(v.pos, v.size * (2.5f - v.life), ImColor(200, 195, 180, (int)(180 * v.life)));
    }

    const char* tL = "WE";
    const char* tR = "ST";
    float fSize = 280.0f;
    ImVec2 sL = ImGui::CalcTextSize(tL);
    float scaleFac = fSize / ImGui::GetFontSize();
    sL.x *= scaleFac; sL.y *= scaleFac;

    ImVec2 basePos = {SCREEN_WIDTH / 2.0f + shake.x, textPosY + shake.y - bounceOffset};

    auto DrawCinematicText = [&](ImVec2 pos, const char* text, float dir) {
        float bst = (slashProgress > 0.05f) ? powf(slashProgress - 0.05f, 1.5f) * 6000.0f : 0.0f;
        float opa = (slashProgress > 0.05f) ? (1.0f - (slashProgress - 0.05f) * 2.5f) : 1.0f;
        if (opa < 0) opa = 0;
        
        ImVec2 dPos = {pos.x + (dir * bst), pos.y + (bst * 0.15f * dir)};

        for(int i = 15; i > 0; i--) {
            ImVec2 off = {(float)i * 0.8f, (float)i * 0.8f};
            drawList->AddText(NULL, fSize, {dPos.x + off.x, dPos.y + off.y}, ImColor(20, 15, 10, (int)(255 * opa)), text);
        }

        for(int i = 8; i > 0; i--) {
            drawList->AddText(NULL, fSize, {dPos.x + i, dPos.y + i}, ImColor(120, 90, 40, (int)(255 * opa)), text);
        }

        drawList->AddText(NULL, fSize, dPos, ImColor(255, 220, 100, (int)(255 * opa)), text);

        if (slashProgress > 0.05f && slashProgress < 0.4f) {
            ImVec2 edgeP1 = {dPos.x, dPos.y + (dir > 0 ? 0 : sL.y)};
            ImVec2 edgeP2 = {dPos.x + sL.x, dPos.y + (dir > 0 ? 0 : sL.y)};
            drawList->AddLine(edgeP1, edgeP2, ImColor(255, 255, 255, (int)(255 * (0.4f - slashProgress))), 5.0f);
        }
    };

    float gap = 40.0f + (slashProgress > 0.05f ? powf(slashProgress - 0.05f, 1.2f) * 3500.0f : 0.0f);
    DrawCinematicText({basePos.x - sL.x - gap, basePos.y - sL.y / 2.0f}, tL, -1.0f);
    DrawCinematicText({basePos.x + gap, basePos.y - sL.y / 2.0f}, tR, 1.0f);

    if (slashProgress > 0.0f && slashAlpha > 0.0f) {
        ImVec2 p1 = { basePos.x - 1200, basePos.y + 300 };
        ImVec2 p2 = { basePos.x + 1200, basePos.y - 300 };
        
        for(int i = 0; i < 3; i++) {
            drawList->AddLine({p1.x, p1.y + i*5}, {p2.x, p2.y + i*5}, ImColor(255, 255, 255, (int)(255 * slashAlpha)), 4.0f);
        }
        drawList->AddLine(p1, p2, ImColor(255, 245, 200, (int)(255 * slashAlpha)), 20.0f);
        drawList->AddLine(p1, p2, ImColor(212, 175, 55, (int)(150 * slashAlpha)), 60.0f);
        
        for(int i = 0; i < 20; i++) {
            float r = (float)(rand() % 100) / 100.0f;
            ImVec2 sp = {p1.x + (p2.x - p1.x) * r, p1.y + (p2.y - p1.y) * r};
            drawList->AddCircleFilled(sp, (float)(rand() % 10 + 5), ImColor(255, 200, 50, (int)(255 * slashAlpha)));
        }
    }

    ImVec2 p = {SCREEN_WIDTH / 2.0f - 450 + shake.x, SCREEN_HEIGHT - 180.0f + shake.y};
    drawList->AddRectFilled(p, {p.x + 900, p.y + 120}, ImColor(40, 30, 20, 180), 10.0f);
    drawList->AddRect(p, {p.x + 900, p.y + 120}, ImColor(212, 175, 55, 200), 10.0f, 0, 4.0f);
    
    char buf[64];
    sprintf(buf, "CORE PRESSURE: %d PSI", (int)(pressureVal * 4500));
    ImGui::SetWindowFontScale(2.5f);
    ImVec2 tSize = ImGui::CalcTextSize(buf);
    ImGui::SetCursorPos({p.x + (900 - tSize.x)/2, p.y + (120 - tSize.y)/2});
    ImGui::TextColored({1.0f, 0.85f, 0.4f, 1.0f}, buf);

    if (impactFlash > 0.0f) {
        drawList->AddRectFilled({0, 0}, {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, ImColor(255, 240, 200, (int)(160 * impactFlash)));
    }

    ImGui::End();
}

bool IsTextFallingFinished() {
    return isFinished;
}