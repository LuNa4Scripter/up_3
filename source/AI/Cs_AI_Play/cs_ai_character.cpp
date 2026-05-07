#include "cs_ai_character.h"
#include "../../data/cs_data.h"

Texture2D aiTex;

void InitAICharacter() {
    aiTex = LoadTexture("assets/ai.png");
}

void UnloadAICharacter() {
    UnloadTexture(aiTex);
}

void DrawAICharacter() {
    for (const auto& enemy : enemies) {
        if (!enemy.active) continue;

        Rectangle sourceRec = { 0.0f, 0.0f, (float)aiTex.width, (float)aiTex.height };
        
        Rectangle destRec = { 
            enemy.pos.x + enemy.size.x / 2.0f, 
            enemy.pos.y + enemy.size.y / 2.0f, 
            enemy.size.x * 2.0f, 
            enemy.size.y * 2.0f 
        };

        Vector2 origin = { destRec.width / 2.0f, destRec.height / 2.0f };
        DrawTexturePro(aiTex, sourceRec, destRec, origin, 180.0f, WHITE); 
    }
}