#ifndef CS_PERFORMANCE_H
#define CS_PERFORMANCE_H

#include "../../include/imgui.h"
#include <vector>

struct Particle {
    ImVec2 pos;
    ImVec2 vel;
    float accY;
    float startSize;
    float endSize;
    float startAlpha;
    float endAlpha;
    float maxLifetime;
    float lifetime;
    ImColor baseColor;
};

extern std::vector<Particle> g_particles;
extern bool g_enableMuzzleSmoke;

void AddMuzzleSmoke(ImVec2 barrelEndPos, float shootAngle);
void UpdateAndDrawParticles(float dt);

#endif // CS_PERFORMANCE_H