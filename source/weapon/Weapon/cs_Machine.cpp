#include "cs_Machine.h"
#include "../../data/cs_data.h"

void FireMachine(Vector2 pos) {
    Object b;
    b.pos = { pos.x + 15 - 2, pos.y }; 
    b.vel = { 0, -800.0f };
    b.size = { 4, 12 };
    b.hp = 1;
    b.active = true;
    
    bullets.push_back(b);
}