#include "cs_ai_manager.h"
#include "../../data/cs_data.h"
#include "../../Server_Optimization/AI_Optimization/cs_ai_optimization.h"
#include <vector>
#include <math.h>

static float ai_timer = 0.0f;
static int frame_counter = 0;

bool IsInView(Vector2 pos, float margin) {
    return (pos.x >= -margin && pos.x <= SCREEN_WIDTH + margin &&
            pos.y >= -margin && pos.y <= SCREEN_HEIGHT + margin);
}

void InitAIManager() {
    enemies.clear();
    ai_timer = 0.0f;
    InitAIOptimizer();
}

void SpawnEnemyCustom(float x, float y, int type, int pattern) {
    Object e = { 0 }; 
    e.pos = { x, y };
    e.vel = { 0, 0 };
    e.size = { 35, 30 };
    e.spd = (float)GetRandomValue(80, 120);
    e.type = type;
    e.hp = 1;
    e.state = 0;
    e.pattern_step = pattern;
    e.timer = 0.0f;
    e.val1 = (float)GetRandomValue(8, 15);
    e.val2 = (float)GetRandomValue(1, 2);
    e.is_dead = false;
    e.active = true;
    e.spawnX = x; 

    enemies.push_back(e);
}

void UpdateAIManager(float dt, float px, float py) {
    frame_counter++;
    ai_timer += dt;

    if (ai_timer > 0.7f) {
        float rx = (float)GetRandomValue(40, SCREEN_WIDTH - 40);
        int rp = GetRandomValue(0, 2);
        SpawnEnemyCustom(rx, -50.0f, 0, rp);
        ai_timer = 0.0f;
    }

    ApplyAI_LOD(enemies, frame_counter);

    for (int i = 0; i < (int)enemies.size(); i++) {
        if (enemies[i].hp <= 0 || enemies[i].is_dead) {
            enemies[i].is_dead = true;
            enemies[i].active = false;
        }

        if (!enemies[i].active) continue;

        enemies[i].timer += dt;
        float t = enemies[i].timer;

        switch (enemies[i].pattern_step) {
            case 0:
                enemies[i].pos.y += enemies[i].spd * dt;
                break;
            case 1:
                enemies[i].pos.y += enemies[i].spd * 0.9f * dt;
                enemies[i].pos.x = enemies[i].spawnX + sinf(t * enemies[i].val2) * enemies[i].val1;
                break;
            case 2:
                enemies[i].pos.y += enemies[i].spd * 1.05f * dt;
                if (enemies[i].pos.y > 150.0f && enemies[i].pos.y < 350.0f) {
                    float dir = (enemies[i].spawnX > (float)SCREEN_WIDTH / 2.0f) ? -1.0f : 1.0f;
                    enemies[i].pos.x += 25.0f * dir * dt;
                }
                break;
        }

        if (enemies[i].pos.y > SCREEN_HEIGHT + 50) {
            enemies[i].is_dead = true;
            enemies[i].active = false;
        }
    }

    if (frame_counter % 20 == 0) {
        FastClearDead(enemies);
    }
}

void DrawAIManager() {
    for (int i = 0; i < (int)enemies.size(); i++) {
        if (enemies[i].active && !enemies[i].is_dead) {
            DrawRectangleV(enemies[i].pos, enemies[i].size, RED);
            DrawRectangleLinesEx({ enemies[i].pos.x, enemies[i].pos.y, enemies[i].size.x, enemies[i].size.y }, 2, MAROON);
        }
    }
}

void KillAll() {
    for (int i = 0; i < (int)enemies.size(); i++) {
        enemies[i].hp = 0;
        enemies[i].is_dead = true;
        enemies[i].active = false;
    }
    FastClearDead(enemies);
}

void ResetManager() {
    InitAIManager();
}