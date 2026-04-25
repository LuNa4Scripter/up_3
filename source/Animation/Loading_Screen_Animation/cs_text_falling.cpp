#include "cs_text_falling.h"
#include "../../data/cs_data.h"
#include "../../include/imgui.h"
#include "cs_shaking.h"
#include <math.h>
#include <vector>
#include <string>

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
static float globalChromaticAberration = 0.0f;
static std::vector<CyberParticle> particles;
static std::vector<Shockwave> shockwaves;

void InitTextFalling() {
    textPosY = -300.0f;
    velocityY = 0.0f;
    slashProgress = 0.0f;
    slashAlpha = 0.0f;
    settleTimer = 0.0f;
    internalTimer = 0.0f;
    impactFlash = 0.0f;
    globalChromaticAberration = 0.0f;
    isFinished = false;
    hasShaked = false;
    particles.clear();
    shockwaves.clear();
}

static void SpawnRefinedParticle(ImVec2 pos, ImVec2 vel, ImU32 color, float size, float life, int type) {
    CyberParticle p;
    p.pos = pos;
    p.vel = vel;
    p.color = color;
    p.size = size;
    p.life = life;
    p.maxLife = life;
    p.type = type;
    p.rotation = (float)(rand() % 360);
    p.rotVel = (float)(rand() % 200 - 100);
    particles.push_back(p);
}

static void CreateShockwave(ImVec2 pos, float maxR, float thick, ImU32 col) {
    Shockwave sw;
    sw.center = pos;
    sw.radius = 0.0f;
    sw.maxRadius = maxR;
    sw.thickness = thick;
    sw.life = 1.0f;
    sw.color = col;
    shockwaves.push_back(sw);
}

void UpdateTextFalling(float dt) {
    internalTimer += dt;
    if (impactFlash > 0.0f) impactFlash -= dt * 3.5f;
    if (globalChromaticAberration > 0.0f) globalChromaticAberration -= dt * 5.0f;

    for (int i = 0; i < (int)shockwaves.size(); i++) {
        shockwaves[i].radius += (shockwaves[i].maxRadius - shockwaves[i].radius) * dt * 12.0f;
        shockwaves[i].life -= dt * 2.0f;
        if (shockwaves[i].life <= 0.0f) {
            shockwaves.erase(shockwaves.begin() + i);
            i--;
        }
    }

    for (int i = 0; i < (int)particles.size(); i++) {
        particles[i].pos.x += particles[i].vel.x * dt;
        particles[i].pos.y += particles[i].vel.y * dt;
        particles[i].vel.x *= (1.0f - dt * 1.5f);
        particles[i].vel.y *= (1.0f - dt * 1.5f);
        particles[i].rotation += particles[i].rotVel * dt;
        particles[i].life -= dt;
        if (particles[i].life <= 0) {
            particles.erase(particles.begin() + i);
            i--;
        }
    }

    if (textPosY < targetY) {
        float gravity = 12000.0f;
        float drag = 0.92f;
        velocityY += gravity * dt;
        velocityY *= (1.0f - (1.0f - drag) * dt * 60.0f);
        textPosY += velocityY * dt;

        if (textPosY >= targetY) {
            textPosY = targetY;
            impactFlash = 1.2f;
            globalChromaticAberration = 1.5f;
            if (!hasShaked) {
                StartImGuiShake(0.5f, 90.0f);
                CreateShockwave(ImVec2(SCREEN_WIDTH / 2.0f, targetY), 800.0f, 12.0f, ImColor(255, 255, 255, 200));
                CreateShockwave(ImVec2(SCREEN_WIDTH / 2.0f, targetY), 1200.0f, 4.0f, ImColor(0, 200, 255, 150));
                for (int i = 0; i < 40; i++) {
                    float ang = (float)(rand() % 360) * 0.0174f;
                    float m = (float)(rand() % 1500 + 500);
                    SpawnRefinedParticle(ImVec2(SCREEN_WIDTH / 2.0f, targetY), ImVec2(cosf(ang) * m, sinf(ang) * m * 0.3f), ImColor(0, 255, 255), 4.0f, 0.6f, 0);
                }
                hasShaked = true;
            }
        }
    } else {
        if (settleTimer < 0.6f) {
            settleTimer += dt;
        } else {
            if (slashProgress == 0.0f) {
                StartImGuiShake(0.8f, 180.0f);
                impactFlash = 0.8f;
                globalChromaticAberration = 2.0f;
                CreateShockwave(ImVec2(SCREEN_WIDTH / 2.0f, targetY), 1500.0f, 20.0f, ImColor(255, 50, 100, 255));
                for (int i = 0; i < 50; i++) {
                    float spd = (float)(rand() % 3000 + 1000);
                    SpawnRefinedParticle(ImVec2(SCREEN_WIDTH / 2.0f, targetY), ImVec2((rand() % 2 == 0 ? 1 : -1) * spd, (float)(rand() % 400 - 200)), ImColor(255, 255, 255), 3.0f, 0.8f, 1);
                }
            }
            slashProgress += dt;
            if (slashProgress < 0.2f) slashAlpha = 1.0f;
            else slashAlpha -= 1.5f * dt;
            if (slashAlpha < 0) slashAlpha = 0;
            if (slashProgress > 2.2f) isFinished = true;
        }
    }
}

void DrawTextFalling() {
    ImVec2 shake = GetImGuiShakeOffset();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT));
    ImGui::Begin("AAA_VFX_Overlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImFont* fontMain = ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : ImGui::GetFont();

    if (impactFlash > 0.0f) {
        drawList->AddRectFilled(ImVec2(0, 0), ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT), ImColor(255, 255, 255, (int)(160 * impactFlash)));
    }

    for (auto& sw : shockwaves) {
        float alpha = sw.life;
        drawList->AddCircle(sw.center, sw.radius, ImColor(sw.color & 0xFFFFFF | ((int)(255 * alpha) << 24)), 128, sw.thickness * alpha);
    }

    for (auto& p : particles) {
        float a = p.life / p.maxLife;
        ImU32 col = p.color & 0xFFFFFF | ((int)(255 * a) << 24);
        if (p.type == 0) {
            drawList->AddCircleFilled(p.pos, p.size, col);
        } else {
            ImVec2 p1 = {p.pos.x - p.size, p.pos.y - p.size};
            ImVec2 p2 = {p.pos.x + p.size, p.pos.y + p.size};
            drawList->AddRectFilled(p1, p2, col, 0.0f);
        }
    }

    const char* tL = "WE";
    const char* tR = "ST";
    float fSize = 220.0f;
    ImVec2 sL = ImGui::CalcTextSize(tL);
    float scaleFac = fSize / ImGui::GetFontSize();
    sL.x *= scaleFac; sL.y *= scaleFac;

    float prog = slashProgress;
    ImVec2 basePos = ImVec2(SCREEN_WIDTH / 2.0f + shake.x, textPosY + shake.y);

    auto DrawHighEndText = [&](ImVec2 pos, const char* text, float dir) {
        float bst = (prog > 0.04f) ? (prog - 0.04f) * 4500.0f : 0.0f;
        float opa = (prog > 0.04f) ? (1.0f - (prog - 0.04f) * 2.2f) : 1.0f;
        if (opa < 0) opa = 0;

        ImVec2 drawPos = ImVec2(pos.x + (dir * bst), pos.y + (bst * 0.12f * dir));
        
        float glitchTime = fmodf(internalTimer, 2.0f);
        bool doGlitch = (glitchTime > 0.1f && glitchTime < 0.15f) || (glitchTime > 1.8f && glitchTime < 1.85f);
        if (doGlitch) {
            drawPos.x += (float)(rand() % 14 - 7);
            globalChromaticAberration += 0.05f;
        }

        float ab = globalChromaticAberration * 5.0f;
        drawList->AddText(fontMain, fSize, {drawPos.x - ab, drawPos.y}, ImColor(255, 0, 80, (int)(130 * opa)), text);
        drawList->AddText(fontMain, fSize, {drawPos.x + ab, drawPos.y}, ImColor(0, 240, 255, (int)(130 * opa)), text);

        for (int i = 1; i <= 4; i++) {
            float gRadius = i * 2.5f;
            drawList->AddText(fontMain, fSize, {drawPos.x - gRadius, drawPos.y}, ImColor(0, 255, 255, (int)(40 * opa / i)), text);
            drawList->AddText(fontMain, fSize, {drawPos.x + gRadius, drawPos.y}, ImColor(0, 255, 255, (int)(40 * opa / i)), text);
        }

        drawList->AddText(fontMain, fSize, drawPos, ImColor(255, 255, 255, (int)(255 * opa)), text);

        float gradY = fmodf(internalTimer * 500.0f, sL.y * 2.0f);
        drawList->PushClipRect({drawPos.x - 10, drawPos.y + gradY - 40}, {drawPos.x + sL.x + 10, drawPos.y + gradY});
        drawList->AddText(fontMain, fSize, drawPos, ImColor(0, 255, 255, (int)(180 * opa)), text);
        drawList->PopClipRect();

        if (prog > 0.04f) {
            float slice = 4.0f;
            for (float sy = 0; sy < sL.y; sy += slice) {
                float shift = sinf(sy * 0.05f + internalTimer * 30.0f) * (prog * 200.0f);
                drawList->PushClipRect({drawPos.x - 300, drawPos.y + sy}, {drawPos.x + sL.x + 300, drawPos.y + sy + slice});
                drawList->AddText(fontMain, fSize, {drawPos.x + shift, drawPos.y}, ImColor(255, 255, 255, (int)(100 * opa)), text);
                drawList->PopClipRect();
            }
        }
    };

    float gap = 20.0f + (prog > 0.04f ? (prog - 0.04f) * 2800.0f : 0.0f);
    DrawHighEndText(ImVec2(basePos.x - sL.x - gap, basePos.y - sL.y / 2.0f), tL, -1.0f);
    DrawHighEndText(ImVec2(basePos.x + gap, basePos.y - sL.y / 2.0f), tR, 1.0f);

    if (prog > 0.0f) {
        ImVec2 cp = ImVec2(SCREEN_WIDTH / 2.0f + shake.x, targetY + shake.y);
        float len = 3500.0f;
        float ang = 0.15f;
        ImVec2 start = {cp.x + len * ang, cp.y - len * 0.5f};
        ImVec2 end = {cp.x - len * ang, cp.y + len * 0.5f};

        for (int i = 0; i < 8; i++) {
            float spread = i * 4.0f * slashAlpha;
            drawList->AddLine({start.x + spread, start.y}, {end.x + spread, end.y}, ImColor(0, 255, 255, (int)(60 * slashAlpha / (i + 1))), (20.0f - i * 2) * slashAlpha);
            drawList->AddLine({start.x - spread, start.y}, {end.x - spread, end.y}, ImColor(255, 0, 100, (int)(60 * slashAlpha / (i + 1))), (20.0f - i * 2) * slashAlpha);
        }

        drawList->AddLine(start, end, ImColor(255, 255, 255, (int)(255 * slashAlpha)), 10.0f * slashAlpha);
        
        for (int i = 0; i < 20; i++) {
            float off = (float)(rand() % 400 - 200) * slashAlpha;
            float thickness = (float)(rand() % 3 + 1);
            drawList->AddLine({start.x + off, start.y}, {end.x + off, end.y}, ImColor(255, 255, 255, (int)(100 * slashAlpha)), thickness);
        }

        for (int i = 0; i < 10; i++) {
            float tOff = i * 20.0f * (1.0f - slashAlpha);
            drawList->AddLine({start.x + tOff, start.y}, {end.x + tOff, end.y}, ImColor(0, 255, 255, (int)(40 * slashAlpha)), 1.5f);
        }
    }

    ImGui::End();
}

bool IsTextFallingFinished() {
    return isFinished;
}