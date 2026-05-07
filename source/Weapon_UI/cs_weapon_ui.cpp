#include "cs_weapon_ui.h"
#include "cs_game_freezes.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include "../player/cs_attack.h"
#include "../data/cs_data.h"

void DrawWeaponUI() {
    static int hoveredWeapon = 0;
    static float animScale = 0.0f;
    static float selectionAngle = 0.0f;
    static float glitchTimer = 0.0f;
    
    const char* weaponNames[] = { 
        "BASIC", "BURST", "MACHINE", "CANNON", "ROTARY", "RPG", "SHOTGUN" 
    };
    int totalWeapons = 7; 
    float angleStep = 360.0f / totalWeapons;
    glitchTimer += GetFrameTime();

    if (!isWeaponMenuOpen) { // isGamePaused 대신 새로 만든 변수 사용
        animScale = Lerp(animScale, 0.0f, 0.25f);
        if (animScale < 0.001f) animScale = 0.0f;
        
        const char* currentName = (currentWeapon >= 0 && currentWeapon < totalWeapons) ? weaponNames[currentWeapon] : "UNKNOWN";
        Rectangle uiRect = { (float)SCREEN_WIDTH - 200, (float)SCREEN_HEIGHT - 70, 185, 55 };
        
        DrawRectangleRounded(uiRect, 0.05f, 5, Fade(BLACK, 0.8f));
        DrawRectangleRoundedLines(uiRect, 0.05f, 5, CLR_MAIN);
        
        for(int i=0; i<3; i++) {
            DrawLineEx({uiRect.x + 5 + i*4, uiRect.y + 5}, {uiRect.x + 5 + i*4, uiRect.y + 15}, 1, CLR_MAIN);
        }
        
        DrawText("WEAPON_SYSTEM_STATUS", (int)uiRect.x + 25, (int)uiRect.y + 8, 10, CLR_MAIN);
        DrawText(currentName, (int)uiRect.x + 15, (int)uiRect.y + 22, 24, WHITE);
        
        if (sinf(glitchTimer * 20) > 0.7f) {
            DrawRectangleRec(uiRect, Fade(CLR_MAIN, 0.1f));
        }
        return;
    }

    animScale = Lerp(animScale, 1.0f, 0.15f);
    if (IsKeyPressed(KEY_D)) hoveredWeapon = (hoveredWeapon + 1) % totalWeapons;
    if (IsKeyPressed(KEY_A)) hoveredWeapon = (hoveredWeapon - 1 + totalWeapons) % totalWeapons;

    float targetAngle = hoveredWeapon * angleStep;
    if (selectionAngle - targetAngle > 180) selectionAngle -= 360;
    else if (targetAngle - selectionAngle > 180) selectionAngle += 360;
    selectionAngle = Lerp(selectionAngle, targetAngle, 0.2f);

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentWeapon = hoveredWeapon;
        isWeaponMenuOpen = false; // 선택 완료 시 메뉴만 닫음
    }

    Vector2 center = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    float outerRadius = 200.0f * animScale;
    float innerRadius = 75.0f * animScale;

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f * animScale));
    
    for(int i=0; i<SCREEN_HEIGHT; i+=4) {
        DrawLine(0, i, SCREEN_WIDTH, i, Fade(CLR_MAIN, 0.05f * animScale));
    }

    for(int i = 0; i < 5; i++) {
        DrawCircleLinesV(center, outerRadius + (i * 2) + sinf(glitchTimer * 12 + i) * 2, Fade(CLR_MAIN, (0.3f - i * 0.05f) * animScale));
    }

    for (int i = 0; i < totalWeapons; i++) {
        float startAngle = i * angleStep - (angleStep / 2.0f) - 90.0f;
        float endAngle = startAngle + angleStep;
        bool isHovered = (hoveredWeapon == i);
        
        if (isHovered) {
            DrawCircleSector(center, outerRadius + 8, startAngle, endAngle, 36, Fade(CLR_MAIN, 0.2f * animScale));
            DrawCircleSectorLines(center, outerRadius + 12, startAngle, endAngle, 36, Fade(CLR_MAIN, 0.8f * animScale));
            
            for(int j=0; j<3; j++) {
                float shiftX = (float)GetRandomValue(-3, 3);
                float shiftY = (float)GetRandomValue(-1, 1);
                DrawCircleSectorLines(center, outerRadius + 12, startAngle, endAngle, 36, Fade(CLR_SUB, (0.4f - j*0.1f) * animScale));
            }
        }

        float rad = (i * angleStep - 90.0f) * DEG2RAD;
        Vector2 textPos = {
            center.x + cosf(rad) * (outerRadius - 70),
            center.y + sinf(rad) * (outerRadius - 70)
        };
        
        int fontSize = isHovered ? 26 : 18;
        int textW = MeasureText(weaponNames[i], fontSize);
        
        if(isHovered && sinf(glitchTimer * 50) > 0.5f) {
            DrawText(weaponNames[i], (int)textPos.x - (textW / 2) + 4, (int)textPos.y - (fontSize/2), fontSize, CLR_SUB);
        }
        DrawText(weaponNames[i], (int)textPos.x - (textW / 2), (int)textPos.y - (fontSize/2), fontSize, isHovered ? WHITE : Fade(GRAY, 0.5f * animScale));
    }

    DrawCircleV(center, innerRadius, Fade(CLR_BG, animScale));
    DrawCircleLinesV(center, innerRadius, CLR_MAIN);
    
    float ringRot = glitchTimer * 70;
    DrawCircleSectorLines(center, innerRadius - 8, ringRot, ringRot + 30, 20, Fade(CLR_MAIN, 0.6f * animScale));
    DrawCircleSectorLines(center, innerRadius - 8, ringRot + 120, ringRot + 150, 20, Fade(CLR_MAIN, 0.6f * animScale));
    DrawCircleSectorLines(center, innerRadius - 8, ringRot + 240, ringRot + 270, 20, Fade(CLR_MAIN, 0.6f * animScale));

    const char* hName = weaponNames[hoveredWeapon];
    int hW = MeasureText(hName, 32);
    DrawText(hName, (int)center.x - (hW / 2), (int)center.y - 16, 32, CLR_MAIN);
    
    DrawText(TextFormat("SYS_TIME: %.2f", glitchTimer), (int)center.x - 40, (int)center.y + 35, 10, Fade(CLR_MAIN, 0.6f * animScale));
}