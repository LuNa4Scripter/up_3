#include "cs_ai_optimization.h"
#include "../../data/cs_data.h"
// #include "../Cs_AI_Manager/cs_ai_manager.h"

AI_Opt_Config opt;

void InitAIOptimizer() {
    opt.max_active_calc = 100;
    opt.lod_dist = 400.0f;
    opt.frame_skip_cnt = 2;
    opt.use_culling = true;
}

bool IsInViewOpt(Vector2 pos, float margin) {
    if (pos.y < -margin || pos.y > SCREEN_HEIGHT + margin) return false;
    if (pos.x < -margin || pos.x > SCREEN_WIDTH + margin) return false;
    return true;
}

void ApplyAI_LOD(std::vector<Object>& enemies, int current_frame) {
    for (int i = 0; i < (int)enemies.size(); i++) {
        if (enemies[i].is_dead || !enemies[i].active) continue;

        if (opt.use_culling && !IsInViewOpt(enemies[i].pos, 50.0f)) {
            if (enemies[i].pos.y > SCREEN_HEIGHT + 10) {
                enemies[i].is_dead = true;
                enemies[i].active = false;
            }
        }
    }
}

void FastClearDead(std::vector<Object>& enemies) {
    for (int i = (int)enemies.size() - 1; i >= 0; i--) {
        if (enemies[i].is_dead || !enemies[i].active) {
            enemies.erase(enemies.begin() + i);
        }
    }
}