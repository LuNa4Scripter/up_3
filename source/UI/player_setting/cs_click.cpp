#include "cs_click.h"
#include "../../data/cs_data.h"

bool isSettingsOpen = false;

void UpdateSettingsInput() {
    if (IsKeyPressed(KEY_P)) {
        isSettingsOpen = !isSettingsOpen;
        isGamePaused = isSettingsOpen;
    }

    Vector2 mousePos = GetMousePosition();
    Rectangle settingsBtn = { (float)GetScreenWidth() - 50.0f, 10.0f, 40.0f, 40.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, settingsBtn)) {
            isSettingsOpen = !isSettingsOpen;
            isGamePaused = isSettingsOpen;
        }
    }
}