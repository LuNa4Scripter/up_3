#include "cs_crash.h"
#include "../data/cs_data.h"

void UpdateCrash() {
    for (int i = 0; i < (int)bullets.size(); i++) {
        for (int j = 0; j < (int)enemies.size(); j++) {
            if (CheckCollisionRecs(
                (Rectangle){ bullets[i].pos.x, bullets[i].pos.y, bullets[i].size.x, bullets[i].size.y },
                (Rectangle){ enemies[j].pos.x, enemies[j].pos.y, enemies[j].size.x, enemies[j].size.y }
            )) {
                bullets.erase(bullets.begin() + i);
                enemies.erase(enemies.begin() + j);
                i--;
                break;
            }
        }
    }
}