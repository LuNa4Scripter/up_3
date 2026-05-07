#include "cs_Rotary.h"
#include "../../data/cs_data.h"
#include <vector>

float rotarySpinSpeed = 0.0f;
float rotaryFireTimer = 0.0f;
const float MAX_SPIN_SPEED = 1.0f;
const float FIRE_RATE = 0.05f;

void UpdateRotary(float dt) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        rotarySpinSpeed += dt * 2.0f; 
        if (rotarySpinSpeed > MAX_SPIN_SPEED) rotarySpinSpeed = MAX_SPIN_SPEED;

        if (rotarySpinSpeed >= 0.8f) {
            rotaryFireTimer += dt;
            if (rotaryFireTimer >= FIRE_RATE) {
                ShootRotary();
                rotaryFireTimer = 0.0f;
            }
        }
    } else {
        rotarySpinSpeed -= dt * 1.5f;
        if (rotarySpinSpeed < 0.0f) rotarySpinSpeed = 0.0f;
        rotaryFireTimer = 0.0f;
    }
}

void ShootRotary() {
    Object b;
    b.pos = (Vector2){ player.pos.x + (player.size.x / 2) - 2, player.pos.y }; 
    b.vel = (Vector2){ 0, -800 }; 
    b.size = (Vector2){ 4, 12 };
    b.active = true;
    b.damage = 10;
    
    bullets.push_back(b);
}

void DrawRotary() {
    if (rotarySpinSpeed > 0.0f) {
        DrawRectangle(player.pos.x - 10, player.pos.y, 5, 30 * rotarySpinSpeed, CLR_MAIN);
    }
}