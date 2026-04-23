#include "cs_rpg.h"
#include "raymath.h"
#include "../../data/cs_data.h"
#include "../../particles/cs_explosion.h"
#include <vector>

struct Rocket {
    Vector2 pos;
    Vector2 vel;
    float speed;
    float acceleration;
};

static std::vector<Rocket> rockets;

void FireRPG(Vector2 pos, Vector2 dir) {
    rockets.push_back({ pos, Vector2Scale(dir, 100.0f), 100.0f, 800.0f });
}

void UpdateRPG(float dt) {
    for (int i = 0; i < (int)rockets.size(); i++) {
        rockets[i].speed += rockets[i].acceleration * dt;
        Vector2 direction = Vector2Normalize(rockets[i].vel);
        rockets[i].vel = Vector2Scale(direction, rockets[i].speed);
        
        rockets[i].pos.x += rockets[i].vel.x * dt;
        rockets[i].pos.y += rockets[i].vel.y * dt;

        for (int j = 0; j < (int)enemies.size(); j++) {
            if (CheckCollisionRecs(
                { rockets[i].pos.x - 10, rockets[i].pos.y - 10, 20, 20 },
                { enemies[j].pos.x, enemies[j].pos.y, enemies[j].size.x, enemies[j].size.y }
            )) {
                AddExplosion(rockets[i].pos);
                
                for (int k = 0; k < (int)enemies.size(); k++) {
                    float dist = Vector2Distance(rockets[i].pos, enemies[k].pos);
                    if (dist < 150.0f) {
                        enemies[k].hp -= 100;
                    }
                }

                rockets.erase(rockets.begin() + i);
                i--;
                break;
            }
        }

        if (i >= 0 && (rockets[i].pos.x < 0 || rockets[i].pos.x > SCREEN_WIDTH || 
            rockets[i].pos.y < 0 || rockets[i].pos.y > SCREEN_HEIGHT)) {
            rockets.erase(rockets.begin() + i);
            i--;
        }
    }
}

void DrawRPG() {
    for (const auto& r : rockets) {
        DrawCircleV(r.pos, 6, WHITE);
        DrawCircleV(r.pos, 4, CLR_MAIN);
        
        if (GetRandomValue(0, 10) > 5) {
            DrawCircleV(Vector2Subtract(r.pos, Vector2Scale(Vector2Normalize(r.vel), 10)), 8, Fade(GRAY, 0.5f));
        }
    }
}