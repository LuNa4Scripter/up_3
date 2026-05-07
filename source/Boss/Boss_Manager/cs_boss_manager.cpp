#include "cs_boss_manager.h"
#include "../../data/cs_data.h"
#include <math.h>
#include <stdlib.h>

Boss gameBoss = { 0 };

void InitBoss() {
    gameBoss.pos = (Vector2){ SCREEN_WIDTH / 2.0f - 150.0f, -200.0f };
    gameBoss.size = (Vector2){ 300.0f, 150.0f };
    gameBoss.hp = 5000.0f;
    gameBoss.maxHp = 5000.0f;
    gameBoss.state = BOSS_APPEARING;
    gameBoss.attackTimer = 0.0f;
    gameBoss.currentPattern = PATTERN_NONE;
}

void UpdateBoss(float dt, Vector2 playerPos) {
    if (gameBoss.state == BOSS_INACTIVE) return;

    if (gameBoss.state == BOSS_APPEARING) {
        gameBoss.pos.y += 100.0f * dt;
        if (gameBoss.pos.y >= 50.0f) {
            gameBoss.state = BOSS_ACTIVE;
        }
        return;
    }

    if (gameBoss.state == BOSS_ACTIVE) {
        gameBoss.attackTimer += dt;

        if (gameBoss.attackTimer >= 1.2f) {
            gameBoss.attackTimer = 0.0f;
            gameBoss.currentPattern = (AttackPattern)(rand() % 3);

            Vector2 center = { gameBoss.pos.x + gameBoss.size.x / 2, gameBoss.pos.y + gameBoss.size.y / 2 };

            if (gameBoss.currentPattern == PATTERN_FAN) {
                float startAngle = 30.0f;
                float endAngle = 150.0f;
                int count = 12;
                for (int i = 0; i < count; i++) {
                    float angle = (startAngle + (endAngle - startAngle) / (count - 1) * i) * DEG2RAD;
                    Vector2 vel = { cosf(angle) * 350.0f, sinf(angle) * 350.0f };
                    SpawnBossBullet(center, vel);
                }
            }
            else if (gameBoss.currentPattern == PATTERN_STRAIGHT) {
                int lines = 7;
                for (int i = 0; i < lines; i++) {
                    Vector2 vel = { 0, 500.0f };
                    Vector2 spawnPos = { gameBoss.pos.x + (gameBoss.size.x / (lines - 1)) * i, gameBoss.pos.y + gameBoss.size.y };
                    SpawnBossBullet(spawnPos, vel);
                }
            }
            else if (gameBoss.currentPattern == PATTERN_CIRCLE) {
                int count = 30;
                for (int i = 0; i < count; i++) {
                    float angle = (360.0f / count * i) * DEG2RAD;
                    Vector2 vel = { cosf(angle) * 280.0f, sinf(angle) * 280.0f };
                    SpawnBossBullet(center, vel);
                }
            }
        }

        if (gameBoss.hp <= 0) {
            gameBoss.state = BOSS_DYING;
        }
    }
}

void DrawBoss() {
    if (gameBoss.state == BOSS_INACTIVE || gameBoss.state == BOSS_DEFEATED) return;

    DrawRectangleV(gameBoss.pos, gameBoss.size, RED);
    
    float healthBarWidth = gameBoss.size.x;
    DrawRectangle(gameBoss.pos.x, gameBoss.pos.y - 40, healthBarWidth, 12, BLACK);
    DrawRectangle(gameBoss.pos.x, gameBoss.pos.y - 40, (healthBarWidth * (gameBoss.hp / gameBoss.maxHp)), 12, GREEN);
    DrawRectangleLines(gameBoss.pos.x, gameBoss.pos.y - 40, healthBarWidth, 12, WHITE);
}

void SpawnBossBullet(Vector2 position, Vector2 velocity) {
    Object b = { 0 };
    b.pos = position;
    b.vel = velocity;
    b.size = (Vector2){ 12, 12 };
    b.active = true;
    b.is_dead = false;
    b.type = 1; 
    
    bullets.push_back(b);
}