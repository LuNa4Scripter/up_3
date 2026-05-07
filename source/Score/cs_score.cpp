#include "cs_score.h"
#include "../data/cs_data.h"

void InitScore() {
    score = 0;
}

void AddScore(int amount) {
    score += amount;
    if (score > hiScore) {
        hiScore = score;
    }
}

void UpdateScoreSystem(float dt) {
    // 나중에 추가예정 (기본 틀만 잡아둔 상태)
}

void DrawScore() {
    DrawText(TextFormat("SCORE: %d", score), 20, 20, 20, RAYWHITE);
}