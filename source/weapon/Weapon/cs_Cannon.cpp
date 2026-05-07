#include "cs_Cannon.h"
#include "../../data/cs_data.h"

void UpdateCannon(float dt) {
}

void FireCannon(Vector2 pos) {
    Object b;
    b.pos = { pos.x + (player.size.x / 2) - 10, pos.y - 20 }; 
    b.vel = { 0, -400 }; 
    b.size = { 20, 30 };
    
    b.hp = 1;
    b.damage = 9999; 
    
    b.active = true;
    
    bullets.push_back(b);
}