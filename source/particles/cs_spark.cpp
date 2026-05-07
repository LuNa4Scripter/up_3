#include "cs_spark.h"
#include <vector>

static std::vector<Spark> sparks;

void AddSpark(Vector2 pos) {
    int count = GetRandomValue(5, 8);
    for (int i = 0; i < count; i++) {
        sparks.push_back({
            pos,
            { (float)GetRandomValue(-400, 400), (float)GetRandomValue(-400, 400) },
            1.0f,
            (float)GetRandomValue(1, 3)
        });
    }
}

void UpdateSparks(float dt) {
    for (int i = 0; i < (int)sparks.size(); i++) {
        sparks[i].pos.x += sparks[i].vel.x * dt;
        sparks[i].pos.y += sparks[i].vel.y * dt;
        
        sparks[i].vel.x *= 0.93f;
        sparks[i].vel.y *= 0.93f;
        
        sparks[i].alpha -= 3.5f * dt;

        if (sparks[i].alpha <= 0) {
            sparks.erase(sparks.begin() + i);
            i--;
        }
    }
}

void DrawSparks() {
    for (const auto& s : sparks) {
        DrawRectangleV(s.pos, { s.size, s.size }, Fade(YELLOW, s.alpha));
    }
}