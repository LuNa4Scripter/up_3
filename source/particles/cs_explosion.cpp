#include "cs_explosion.h"
#include <vector>
#include <cmath>

struct ExpParticle {
    Vector2 pos;
    Vector2 vel;
    float size;
    float life;
    Color color;
};

static std::vector<ExpParticle> particles;

void AddExplosion(Vector2 pos) {
    for (int i = 0; i < 40; i++) {
        float angle = (float)GetRandomValue(0, 360) * (PI / 180.0f);
        float speed = (float)GetRandomValue(200, 600);
        
        particles.push_back({
            pos,
            { cosf(angle) * speed, sinf(angle) * speed },
            (float)GetRandomValue(3, 7),
            1.0f,
            (GetRandomValue(0, 1) == 0) ? ORANGE : YELLOW
        });
    }
}

void UpdateExplosions(float dt) {
    for (int i = 0; i < (int)particles.size(); i++) {
        particles[i].vel.x *= 0.92f; 
        particles[i].vel.y *= 0.92f;
        particles[i].vel.y += 500.0f * dt; 
        
        particles[i].pos.x += particles[i].vel.x * dt;
        particles[i].pos.y += particles[i].vel.y * dt;
        
        particles[i].life -= 2.0f * dt;

        if (particles[i].life <= 0) {
            particles.erase(particles.begin() + i);
            i--;
        }
    }
}

void DrawExplosions() {
    for (const auto& p : particles) {
        Color c = p.color;
        if (p.life < 0.4f) c = DARKGRAY;
        
        DrawRectangleV(p.pos, { p.size * p.life, p.size * p.life }, Fade(c, p.life));
        
        if (p.life > 0.6f) {
            DrawCircleV(p.pos, p.size * 2.0f, Fade(p.color, p.life * 0.3f));
        }
    }
}