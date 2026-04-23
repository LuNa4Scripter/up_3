#include "cs_implementation.h"
#include "../../UI/Heart_UI/cs_heart.h"

struct Player {
    float hp;
    float maxHp;
};
extern Player player;

static bool isVisible = false;
static float regenTimer = 0.0f;

void TriggerHeartUI() {
    isVisible = true;
}

void UpdatePlayerHeartLogic(float dt) {
    if (!isVisible) return;

    regenTimer += dt;
    if (regenTimer >= 10.0f) {
        if (player.hp < player.maxHp) {
            player.hp += 5.0f;
            if (player.hp > player.maxHp) player.hp = player.maxHp;
        }
        regenTimer = 0.0f;
    }
}