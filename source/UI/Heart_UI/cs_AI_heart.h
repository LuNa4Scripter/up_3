#ifndef CS_AI_HEART_H
#define CS_AI_HEART_H

#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h"

struct EnemyStatus {
    int unitId;
    float currentHp;
    float maxHp;
    bool isActive;
};

void DrawAiHeartUI(EnemyStatus* units, int count);

#endif