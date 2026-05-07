#ifndef CS_AI_OPTIMIZATION_H
#define CS_AI_OPTIMIZATION_H

#include "../../data/cs_data.h"
#include <vector>

struct AI_Opt_Config {
    int max_active_calc;
    float lod_dist;
    int frame_skip_cnt;
    bool use_culling;
};

void InitAIOptimizer();
// 여기 인자들도 EnemyAI -> Object로 변경!
void ApplyAI_LOD(std::vector<Object>& enemies, int current_frame);
void FastClearDead(std::vector<Object>& enemies);

#endif