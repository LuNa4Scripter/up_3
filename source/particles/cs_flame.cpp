#include "cs_flame.h"
#include <cmath>

std::vector<FlameParticle> flames;

void AddEngineFlame(Vector2 position, float rotation) {
    for (int i = 0; i < 3; i++) {
        FlameParticle p;
        p.pos = { position.x + (float)GetRandomValue(-3, 3), position.y };
        
        p.vel.x = (float)GetRandomValue(-15, 15);
        p.vel.y = (float)GetRandomValue(100, 200); 
        
        p.maxLife = (float)GetRandomValue(4, 8) / 10.0f;
        p.life = p.maxLife;
        
        p.color = WHITE; 
        flames.push_back(p);
    }
}

void UpdateFlames(float dt) {
    for (int i = 0; i < (int)flames.size(); i++) {
        flames[i].pos.x += flames[i].vel.x * dt;
        flames[i].pos.y += flames[i].vel.y * dt;
        
        flames[i].vel.y *= 0.95f;
        
        flames[i].life -= dt;

        float age = flames[i].life / flames[i].maxLife;
        if (age > 0.8f) flames[i].color = WHITE;
        else if (age > 0.5f) flames[i].color = GOLD;
        else if (age > 0.2f) flames[i].color = ORANGE;
        else flames[i].color = RED;

        if (flames[i].life <= 0) {
            flames.erase(flames.begin() + i);
            i--;
        }
    }
}

void DrawFlames() {
    BeginBlendMode(BLEND_ADDITIVE); 
    for (const auto& p : flames) {
        float age = p.life / p.maxLife;
        float size = 6.0f * age;
        
        DrawCircleV(p.pos, size, Fade(p.color, age));
        DrawCircleV(p.pos, size * 0.5f, Fade(WHITE, age * 0.5f));
    }
    EndBlendMode();
}