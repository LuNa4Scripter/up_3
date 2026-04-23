#include "cs_Machine_Use.h"
#include "../../data/cs_data.h"
#include "../Weapon/cs_Machine.h"

static float machineCooldown = 0.0f;

void UpdateMachineUse(float dt, Vector2 playerPos) {
    if (machineCooldown > 0) machineCooldown -= dt;

    if (currentWeapon == 0 || currentWeapon == 2) {
        if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_E)) && machineCooldown <= 0) {
            FireMachine(playerPos);
            machineCooldown = 0.08f; // 연사 속도
        }
    }
}