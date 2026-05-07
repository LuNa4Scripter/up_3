#ifndef CS_BOSS_MANAGER_H
#define CS_BOSS_MANAGER_H

#include "raylib.h"
#include <vector>

enum BossState {
    BOSS_INACTIVE,
    BOSS_APPEARING,
    BOSS_ACTIVE,
    BOSS_DYING,
    BOSS_DEFEATED
};

enum AttackPattern {
    PATTERN_FAN,
    PATTERN_STRAIGHT,
    PATTERN_CIRCLE,
    PATTERN_NONE
};

struct Boss {
    Vector2 pos;
    Vector2 size;
    float hp;
    float maxHp;
    BossState state;
    float attackTimer;
    AttackPattern currentPattern;
};

extern Boss gameBoss;

void InitBoss();
void UpdateBoss(float dt, Vector2 playerPos);
void DrawBoss();
void SpawnBossBullet(Vector2 position, Vector2 velocity);

#endif