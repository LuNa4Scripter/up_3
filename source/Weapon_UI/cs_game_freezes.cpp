#include "cs_game_freezes.h"
#include "../data/cs_data.h"

// bool isGamePaused = false;

void UpdateGameFreeze() {
    if (IsKeyPressed(KEY_Q)) {
        isGamePaused = !isGamePaused;
    }
}