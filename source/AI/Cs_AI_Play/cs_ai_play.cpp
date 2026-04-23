#include "cs_ai_play.h"
#include "../../data/cs_data.h"
#include "../../Server_Optimization/AI_Optimization/cs_ai_optimization.h"
#include <math.h>

static int internal_frame = 0; 

void ProcessAIBehavior(float dt) {
    internal_frame++;
    
    ApplyAI_LOD(enemies, internal_frame);

    for (int i = 0; i < (int)enemies.size(); i++) {
        if (!enemies[i].active || enemies[i].is_dead) continue;

        if (enemies[i].pattern_step != 2 && (i % 2 == internal_frame % 2)) {
            enemies[i].pos.y += (enemies[i].spd * dt * diff);
            continue;
        }

        enemies[i].timer += dt;
        float t = enemies[i].timer;

        switch (enemies[i].pattern_step) {
            case 0:
                enemies[i].pos.y += enemies[i].spd * dt * diff;
                break;
            case 1:
                enemies[i].pos.y += (enemies[i].spd * 0.7f) * dt * diff;
                enemies[i].pos.x = enemies[i].spawnX + sinf(t * enemies[i].val2) * enemies[i].val1;
                break;
            case 2:
                if (enemies[i].state == 0) {
                    enemies[i].pos.y += enemies[i].spd * 1.3f * dt * diff;
                    if (enemies[i].pos.y > 170.0f) enemies[i].state = 1;
                } else {
                    float dir = (enemies[i].spawnX > (float)SCREEN_WIDTH / 2.0f) ? -1.0f : 1.0f;
                    enemies[i].pos.x += 135.0f * dir * dt * diff;
                    enemies[i].pos.y += (enemies[i].spd * 0.45f) * dt * diff;
                }
                break;
        }

        if (enemies[i].pos.y > SCREEN_HEIGHT + 100) {
            enemies[i].is_dead = true;
            enemies[i].active = false;
        }
    }
}