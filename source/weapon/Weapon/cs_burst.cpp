#include "cs_burst.h"
#include "../../data/cs_data.h"

static int burstCount = 0;
static float burstTimer = 0.0f;

void UpdateBurst(float dt) {
    if (burstCount > 0) {
        burstTimer -= dt;
        if (burstTimer <= 0) {
            Object b = { 0 };
            b.pos = { player.pos.x + (player.size.x / 2) - 2, player.pos.y };
            b.vel = { 0, -850.0f };
            b.size = { 5, 15 };
            b.hp = 1;
            b.damage = 1;
            b.type = 1;
            b.active = true;
            
            bullets.push_back(b);

            burstCount--;
            burstTimer = 0.06f;
        }
    }
}

void FireBurst() {
    if (burstCount <= 0) {
        burstCount = 3;
        burstTimer = 0.0f;
    }
}