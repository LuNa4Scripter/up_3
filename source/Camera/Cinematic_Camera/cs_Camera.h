#ifndef CS_CAMERA_H
#define CS_CAMERA_H

#include "raylib.h"

void UpdateScreenShake(float dt);
void BeginScreenShake(float intensity, float duration);
Vector2 GetCameraOffset();

#endif