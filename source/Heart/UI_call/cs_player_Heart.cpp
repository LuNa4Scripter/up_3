#include "cs_player_Heart.h"
#include "../../UI/Heart_UI/cs_heart.h"
#include "../../player/cs_player_character.h"

struct Player {
    float hp;
    float maxHp;
};

extern Player player;

void UpdateAndDrawPlayerHeart() {
    DrawPlayerHeartImgui(player.hp, player.maxHp);
}