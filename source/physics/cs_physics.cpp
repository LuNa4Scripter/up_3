#include "cs_physics.h"
#include "../data/cs_data.h"
#include "../particles/cs_spark.h"
#include "../particles/cs_explosion.h"
#include "../Score/cs_score.h"
#include <vector>

struct Debris {
    Vector2 pos;
    Vector2 vel;
    float alpha;
};

static std::vector<Debris> debrisList;

void UpdatePhysics(float dt) {
    for (int i = 0; i < (int)bullets.size(); i++) {
        for (int j = 0; j < (int)enemies.size(); j++) {
            if (CheckCollisionRecs(
                (Rectangle){ bullets[i].pos.x, bullets[i].pos.y, bullets[i].size.x, bullets[i].size.y },
                (Rectangle){ enemies[j].pos.x, enemies[j].pos.y, enemies[j].size.x, enemies[j].size.y }
            )) {
                AddSpark(bullets[i].pos);
                
                enemies[j].hp = 0;
                enemies[j].is_dead = true;
                
                int currentBulletDamage = bullets[i].damage;
                bullets.erase(bullets.begin() + i);
                i--;

                if (enemies[j].hp <= 0) {
                    AddExplosion(enemies[j].pos);
                    AddScore(10); 
                    
                    int count = (currentBulletDamage >= 9000) ? 20 : 6;
                    for (int k = 0; k < count; k++) {
                        debrisList.push_back({ 
                            enemies[j].pos, 
                            { (float)GetRandomValue(-200, 200), (float)GetRandomValue(-200, 200) }, 
                            1.0f 
                        });
                    }

                    enemies[j].active = false;
                    enemies.erase(enemies.begin() + j);
                    j--;
                }
                break;
            }
        }
    }

    for (int i = 0; i < (int)debrisList.size(); i++) {
        debrisList[i].pos.x += debrisList[i].vel.x * dt;
        debrisList[i].pos.y += debrisList[i].vel.y * dt;
        debrisList[i].alpha -= 1.0f * dt;

        if (debrisList[i].alpha <= 0) {
            debrisList.erase(debrisList.begin() + i);
            i--;
        }
    }
}

void DrawPhysics() {
    for (const auto& d : debrisList) {
        DrawRectangleV(d.pos, { 4, 4 }, Fade(MAROON, d.alpha));
    }
}