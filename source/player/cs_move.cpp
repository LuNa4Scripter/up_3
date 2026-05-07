#include "cs_move.h"
#include "../data/cs_data.h"
#include "../particles/cs_flame.h"
#include "../Animation/cs_move_Animation.h"
#include "raymath.h"

void UpdatePlayerMove(float dt) {
    float baseSpeed = 350.0f; 
    bool isMoving = false;
    
    float targetY = SCREEN_HEIGHT - 100.0f;

    if (dt > 0.1f) dt = 0.1f;

    if (IsKeyDown(KEY_A) && player.pos.x > 0) {
        player.pos.x -= baseSpeed * dt;
        targetRotation = -20.0f;
        isMoving = true;
    }
    if (IsKeyDown(KEY_D) && player.pos.x < SCREEN_WIDTH - player.size.x) {
        player.pos.x += baseSpeed * dt;
        targetRotation = 20.0f;
        isMoving = true;
    }

    player.pos.x += player.vel.x * dt;
    player.pos.y += player.vel.y * dt;

    player.vel.x = Lerp(player.vel.x, 0.0f, 6.0f * dt);
    player.vel.y = Lerp(player.vel.y, 0.0f, 6.0f * dt);

    player.pos.y = Lerp(player.pos.y, targetY, 4.0f * dt);

    if (fabsf(player.vel.x) < 0.1f) player.vel.x = 0;
    if (fabsf(player.vel.y) < 0.1f) player.vel.y = 0;

    player.pos.x = Clamp(player.pos.x, 0, SCREEN_WIDTH - player.size.x);
    player.pos.y = Clamp(player.pos.y, 0, SCREEN_HEIGHT - player.size.y);

    if (!isMoving) {
        targetRotation = 0.0f;
    }

    AddEngineFlame({ player.pos.x + player.size.x / 2, player.pos.y + player.size.y }, playerRotation);
}