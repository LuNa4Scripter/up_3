#include "Heart_Manager.h"
#include <iostream>

std::vector<AI_Heart_Data> ai_hearts;

void InitAIHeartManager() {
    ai_hearts.clear();
}

void RegisterAIHeart(int id, float hp, Rectangle box) {
    AI_Heart_Data data;
    data.id = id;
    data.max_hp = hp;
    data.current_hp = hp;
    data.display_hp = hp;
    data.is_active = true;
    data.hit_box = box;
    ai_hearts.push_back(data);
}

void UpdateAIHearts(float dt) {
    for (int i = 0; i < (int)ai_hearts.size(); i++) {
        if (!ai_hearts[i].is_active) continue;

        if (ai_hearts[i].display_hp > ai_hearts[i].current_hp) {
            ai_hearts[i].display_hp -= 60.0f * dt;
            if (ai_hearts[i].display_hp < ai_hearts[i].current_hp) {
                ai_hearts[i].display_hp = ai_hearts[i].current_hp;
            }
        }

        if (ai_hearts[i].current_hp <= 0) {
            ai_hearts[i].current_hp = 0;
            ai_hearts[i].is_active = false;
        }
    }
}

void ApplyDamageToAI(int id, float damage) {
    for (int i = 0; i < (int)ai_hearts.size(); i++) {
        if (ai_hearts[i].id == id && ai_hearts[i].is_active) {
            ai_hearts[i].current_hp -= damage;
            break;
        }
    }
}

void DrawAIHeartUI() {
    for (int i = 0; i < (int)ai_hearts.size(); i++) {
        if (!ai_hearts[i].is_active) continue;

        Vector2 pos = { ai_hearts[i].hit_box.x, ai_hearts[i].hit_box.y - 15 };
        float width = ai_hearts[i].hit_box.width;
        
        DrawRectangleV(pos, { width, 6 }, BLACK);
        
        float display_w = (ai_hearts[i].display_hp / ai_hearts[i].max_hp) * width;
        DrawRectangleV(pos, { display_w, 6 }, MAROON);
        
        float current_w = (ai_hearts[i].current_hp / ai_hearts[i].max_hp) * width;
        DrawRectangleV(pos, { current_w, 6 }, RED);
    }
}

void ProcessAiHeartSystem(std::vector<Object>& enemyList) {
    if (enemyList.empty()) return;

    UpdateAIHearts(GetFrameTime());
    DrawAIHeartUI();
}