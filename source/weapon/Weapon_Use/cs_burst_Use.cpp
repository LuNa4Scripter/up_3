#include "cs_burst_Use.h"
#include "../../data/cs_data.h"
#include "../Weapon/cs_burst.h"

static float burstWeaponCooldown = 0.0f;

void UpdateBurstUse(float dt, Vector2 playerPos) {
    if (burstWeaponCooldown > 0) burstWeaponCooldown -= dt;
    
    if (currentWeapon == 1) {
        if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_E)) && burstWeaponCooldown <= 0) {
            FireBurst();
            burstWeaponCooldown = 0.6f; 
        }
    }
    UpdateBurst(dt); 
}