#include "cs_shotgun_Use.h"
#include "../Weapon/cs_shotgun.h"
#include "../../data/cs_data.h"
#include "raymath.h"

static float shotTimer = 0.0f;
static const float shotDelay = 0.6f; 
static const float recoilPower = 500.0f;

void HandleShotgunInput() {
    if (currentWeapon == 6) {
        if (shotTimer <= 0.0f && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE))) {
            Vector2 mousePos = GetMousePosition();
            Vector2 playerCenter = { player.pos.x + player.size.x / 2, player.pos.y + player.size.y / 2 };
            Vector2 dir = Vector2Normalize(Vector2Subtract(mousePos, playerCenter));
            
            FireShotgun(playerCenter, dir);
            player.vel.x -= dir.x * recoilPower;
            player.vel.y -= dir.y * recoilPower;
            
            shotTimer = shotDelay;
        }
    }
}

void UpdateShotgunSystem(float dt) {
    if (shotTimer > 0.0f) shotTimer -= dt;
    
    player.vel.x = Lerp(player.vel.x, 0, 0.1f);
    player.vel.y = Lerp(player.vel.y, 0, 0.1f);
}