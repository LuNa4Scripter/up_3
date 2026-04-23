#ifndef CS_AI_MANAGER_H
#define CS_AI_MANAGER_H

#include <vector>
#include "raylib.h"
#include "../../data/cs_data.h"

void InitAIManager();
void UpdateAIManager(float dt, float px, float py);
void DrawAIManager();
void SpawnEnemyCustom(float x, float y, int type, int pattern);
void KillAll();
void ResetManager();

#endif