#ifndef CS_MOVE_ANIMATION_H
#define CS_MOVE_ANIMATION_H

#include "raylib.h"

extern float playerRotation;
extern float targetRotation;

void UpdateMoveAnimation(float dt);
void DrawMovePlayer();

#endif