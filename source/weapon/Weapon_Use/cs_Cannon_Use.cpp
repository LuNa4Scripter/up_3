#include "cs_Cannon_Use.h"
#include "../../data/cs_data.h"
#include "../Weapon/cs_Cannon.h"

static float cannonCooldown = 0.0f;

void UpdateCannonUse(float dt, Vector2 playerPos) {
    if (cannonCooldown > 0) cannonCooldown -= dt;

    if (currentWeapon == 3) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && cannonCooldown <= 0) {
            FireCannon(playerPos);
            cannonCooldown = 0.8f;
        }
    }
}