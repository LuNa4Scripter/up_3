#include "cs_move_Animation.h"
#include "../data/cs_data.h"

float playerRotation = 0.0f;
float targetRotation = 0.0f;

void UpdateMoveAnimation(float dt) {
    if (IsKeyDown(KEY_A)) targetRotation = -20.0f;
    else if (IsKeyDown(KEY_D)) targetRotation = 20.0f;
    else targetRotation = 0.0f;

    playerRotation += (targetRotation - playerRotation) * 8.0f * dt;
}

void DrawMovePlayer() {
    Rectangle rec = { 
        player.pos.x + player.size.x / 2, 
        player.pos.y + player.size.y / 2, 
        player.size.x, 
        player.size.y 
    };
    
    Vector2 origin = { player.size.x / 2, player.size.y / 2 };
    
    DrawRectanglePro(rec, origin, playerRotation, CLR_MAIN);
}