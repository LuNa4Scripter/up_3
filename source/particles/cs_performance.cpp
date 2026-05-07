#include "cs_performance.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

extern Camera2D screenCamera;

std::vector<Particle> g_particles;
bool g_enableMuzzleSmoke = true;

static float RandFloat(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

void AddMuzzleSmoke(ImVec2 barrelEndPos, float shootAngle) {
    if (!g_enableMuzzleSmoke) return;

    int particleCount = (int)RandFloat(20.0f, 30.0f);
    for (int i = 0; i < particleCount; i++) {
        Particle p;
        p.pos = barrelEndPos;

        float angleVar = RandFloat(-0.5f, 0.5f);
        float finalAngle = shootAngle + angleVar;
        float speed = RandFloat(200.0f, 600.0f);
        
        p.vel.x = cosf(finalAngle) * speed;
        p.vel.y = sinf(finalAngle) * speed;
        
        p.accY = RandFloat(-30.0f, -80.0f);
        p.startSize = RandFloat(10.0f, 18.0f);
        p.endSize = p.startSize * RandFloat(4.0f, 8.0f);
        p.startAlpha = RandFloat(0.5f, 0.8f);
        p.endAlpha = 0.0f;
        p.maxLifetime = RandFloat(0.6f, 1.1f);
        p.lifetime = 0.0f;

        float gray = RandFloat(200.0f, 245.0f);
        p.baseColor = ImColor((int)gray, (int)gray, (int)gray);

        g_particles.push_back(p);
    }
}

void UpdateAndDrawParticles(float dt) {
    if (dt <= 0.0f || g_particles.empty()) return;

    ImDrawList* d = ImGui::GetForegroundDrawList();
    
    for (int i = 0; i < (int)g_particles.size(); ) {
        Particle& p = g_particles[i];
        p.lifetime += dt;

        if (p.lifetime >= p.maxLifetime) {
            g_particles.erase(g_particles.begin() + i);
            continue; 
        }

        float t = p.lifetime / p.maxLifetime;
        float drag = 4.0f;
        p.vel.x -= p.vel.x * drag * dt;
        p.vel.y -= p.vel.y * drag * dt;
        p.vel.y += p.accY * dt;

        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;

        float size = p.startSize + (p.endSize - p.startSize) * t;
        float alpha = p.startAlpha * (1.0f - t); 
        
        ImColor c = p.baseColor;
        c.Value.w = alpha;

        Vector2 worldPos = { p.pos.x, p.pos.y };
        Vector2 screenPos = GetWorldToScreen2D(worldPos, screenCamera);

        ImVec2 drawPos = { screenPos.x, screenPos.y };

        ImColor borderColor = ImColor(60, 60, 60, (int)(alpha * 110));
        d->AddCircleFilled(drawPos, size + 1.2f, borderColor, 12);
        d->AddCircleFilled(drawPos, size, c, 12);
        
        i++; 
    }
}