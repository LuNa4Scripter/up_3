#ifndef CS_FLAME_H
#define CS_FLAME_H

#include "raylib.h"
#include <vector>

struct FlameParticle {
    Vector2 pos;
    Vector2 vel;
    float life;
    float maxLife;
    Color color;
};

void AddEngineFlame(Vector2 position, float rotation);
void UpdateFlames(float dt);
void DrawFlames();

#endif