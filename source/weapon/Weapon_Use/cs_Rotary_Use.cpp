#include "cs_Rotary_Use.h"
#include "../Weapon/cs_Rotary.h"
#include "../../data/cs_data.h"

void HandleRotaryInput(float dt) {
    if (currentWeapon == 4) { 
        UpdateRotary(dt);
    }
}

void RenderRotaryEffect() {
    if (currentWeapon == 4) {
        DrawRotary();
        
        Vector2 mousePos = GetMousePosition();
        DrawCircleLinesV(mousePos, 15, CLR_MAIN);
        DrawLine((int)mousePos.x - 20, (int)mousePos.y, (int)mousePos.x + 20, (int)mousePos.y, CLR_MAIN);
        DrawLine((int)mousePos.x, (int)mousePos.y - 20, (int)mousePos.x, (int)mousePos.y + 20, CLR_MAIN);
    }
}