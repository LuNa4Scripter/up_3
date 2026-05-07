#ifndef CS_EARTH_SHAKING_H
#define CS_EARTH_SHAKING_H

#include "raylib.h"

void StartEarthShake(float duration, float intensity);
void UpdateEarthShake(float dt);
Vector2 GetEarthShakeOffset();

#endif