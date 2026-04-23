#ifndef CS_SPARK_H
#define CS_SPARK_H

#include "raylib.h"
#include <vector>

struct Spark {
    Vector2 pos;
    Vector2 vel;
    float alpha;
    float size;
};

void AddSpark(Vector2 pos);
void UpdateSparks(float dt);
void DrawSparks();

#endif