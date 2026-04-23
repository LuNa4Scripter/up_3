#ifndef CS_SHAKING_H
#define CS_SHAKING_H

#include "../../include/imgui.h"

void StartImGuiShake(float duration, float intensity);
void UpdateImGuiShake(float dt);
ImVec2 GetImGuiShakeOffset();

#endif