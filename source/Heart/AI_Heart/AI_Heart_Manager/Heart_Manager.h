#ifndef HEART_MANAGER_H
#define HEART_MANAGER_H

#include "raylib.h"
#include <vector>
#include "../../../data/cs_data.h"

struct AI_Heart_Data {
    int id;
    float max_hp;
    float current_hp;
    float display_hp;
    bool is_active;
    Rectangle hit_box;
};

extern std::vector<AI_Heart_Data> ai_hearts;

void ProcessAiHeartSystem(std::vector<Object>& enemyList); 

void InitAIHeartManager();
void RegisterAIHeart(int id, float hp, Rectangle box);
void UpdateAIHearts(float dt);
void ApplyDamageToAI(int id, float damage);
void DrawAIHeartUI();

#endif