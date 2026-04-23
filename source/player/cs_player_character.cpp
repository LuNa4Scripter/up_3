#include "cs_player_character.h"
#include "../data/cs_data.h"
#include "../Animation/cs_move_Animation.h"

Texture2D playerTex;

void InitPlayerCharacter() {
    playerTex = LoadTexture("assets/player.png");
}

void UnloadPlayerCharacter() {
    UnloadTexture(playerTex);
}

void DrawPlayerCharacter() {
    if (!player.active) return;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)playerTex.width, (float)playerTex.height };
    
    Rectangle destRec = { 
        player.pos.x + player.size.x / 2.0f, 
        player.pos.y + player.size.y / 2.0f, 
        player.size.x * 4.0f, 
        player.size.y * 3.0f 
    };

    Vector2 origin = { destRec.width / 2.0f, destRec.height / 2.0f };

    DrawTexturePro(playerTex, sourceRec, destRec, origin, playerRotation, WHITE);
}