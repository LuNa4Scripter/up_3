#include "cs_shotgun.h"
#include "raymath.h"
#include "../../data/cs_data.h"
#include <vector>

Vector2 shotgunPos = { 0, 0 };
float shotgunRecoilY = 0.0f;

void FireShotgun(Vector2 pos, Vector2 dir) {
    int pelletCount = 7; 
    float spreadAngle = 30.0f; 

    for (int i = 0; i < pelletCount; i++) {
        float randomOffset = (float)GetRandomValue(-spreadAngle / 2, spreadAngle / 2);
        float baseAngle = atan2f(dir.y, dir.x) * RAD2DEG;
        float finalAngle = (baseAngle + randomOffset) * DEG2RAD;

        Vector2 finalDir = { cosf(finalAngle), sinf(finalAngle) };
        
        bullets.push_back({
            pos,
            Vector2Scale(finalDir, 600.0f),
            { 4, 4 },
            true
        });
    }

    shotgunRecoilY = -25.0f; 
}

void UpdateShotgun(float dt) {
    shotgunPos.x = player.pos.x;
    shotgunPos.y = player.pos.y;

    if (shotgunRecoilY < 0) {
        shotgunRecoilY += 150.0f * dt;
        if (shotgunRecoilY > 0) shotgunRecoilY = 0;
    }
}

void DrawShotgun() {
    if (currentWeapon == 2) { 
        Vector2 finalDrawPos = { shotgunPos.x + 10, shotgunPos.y + 10 + shotgunRecoilY };
        DrawRectangleV(finalDrawPos, { 40, 15 }, SKYBLUE);
    }
}