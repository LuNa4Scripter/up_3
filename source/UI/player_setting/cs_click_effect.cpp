#include "cs_click_effect.h"
#include "cs_click.h"
#include "../../data/cs_data.h"

void DrawSettingsOverlay() {
    Vector2 mousePos = GetMousePosition();
    Rectangle settingsBtn = { (float)SCREEN_WIDTH - 50.0f, 10.0f, 40.0f, 40.0f };

    if (!isSettingsOpen) {
        Color btnColor = CLR_MAIN;
        if (CheckCollisionPointRec(mousePos, settingsBtn)) btnColor = YELLOW;
        
        DrawRectangleLinesEx(settingsBtn, 2.0f, btnColor);
        DrawText("P", SCREEN_WIDTH - 35, 20, 20, btnColor);
        return;
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.6f));
}