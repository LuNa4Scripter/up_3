#include "cs_rpg_Use.h"
#include "../Weapon/cs_rpg.h"
#include "../../data/cs_data.h"
#include "raymath.h"

static float shotTimer = 0.0f;
static const float shotDelay = 1.2f;

void HandleRPGInput() {
    if (currentWeapon == 5) {
        if (shotTimer <= 0.0f && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE))) {
            Vector2 mousePos = GetMousePosition();
            Vector2 playerPos = { player.pos.x + player.size.x / 2, player.pos.y + player.size.y / 2 };
            Vector2 dir = Vector2Normalize(Vector2Subtract(mousePos, playerPos));
            
            FireRPG(playerPos, dir);
            shotTimer = shotDelay;
        }
    }
}

void UpdateRPGSystem(float dt) {
    if (shotTimer > 0.0f) shotTimer -= dt;
    UpdateRPG(dt);
}

void DrawRPGSystem() {
    DrawRPG();
    
    if (currentWeapon == 5 && shotTimer > 0.0f) {
        float reloadProgress = 1.0f - (shotTimer / shotDelay);
        DrawRectangleLines(player.pos.x, player.pos.y - 15, 40, 5, DARKGRAY);
        DrawRectangle(player.pos.x, player.pos.y - 15, (int)(40 * reloadProgress), 5, CLR_MAIN);
    }
}