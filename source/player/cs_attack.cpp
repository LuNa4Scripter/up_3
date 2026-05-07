#include "cs_attack.h"
#include "../data/cs_data.h"
#include "../weapon/Weapon_Use/cs_Machine_Use.h"
#include "../weapon/Weapon_Use/cs_burst_Use.h"
#include "../weapon/Weapon_Use/cs_Cannon_Use.h"
#include "../weapon/Weapon_Use/cs_Rotary_Use.h"
#include "../weapon/Weapon_Use/cs_rpg_Use.h"
#include "../weapon/Weapon_Use/cs_shotgun_Use.h"

void UpdatePlayerAttack(float dt) {
    switch (currentWeapon) {
        case 0: UpdateMachineUse(dt, player.pos); break;
        case 1: UpdateBurstUse(dt, player.pos); break;
        case 2: UpdateMachineUse(dt, player.pos); break; 
        case 3: UpdateCannonUse(dt, player.pos); break;
        case 4: HandleRotaryInput(dt); break;
        case 5: HandleRPGInput(); UpdateRPGSystem(dt); break;
        case 6: HandleShotgunInput(); UpdateShotgunSystem(dt); break;
        default: UpdateMachineUse(dt, player.pos); break;
    }
}