#ifndef CS_DATA_H
#define CS_DATA_H

#include "raylib.h"
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 60

enum GameState { 
    TITLE, 
    LOADING,
    MAP_SELECT,
    PLAYING, 
    GAMEOVER, 
    RANKING, 
};

struct Object {
    Vector2 pos;
    Vector2 vel;
    Vector2 size;
    int hp;
    int damage;
    int type;
    float cooldown;
    bool active;

    int pattern_step;
    int state;
    float timer;
    float val1;
    float val2;
    float spawnX;
    float spd;
    bool is_dead;
};

extern GameState state;
extern int score;
extern int hiScore;
extern float timer;
extern float diff;
extern bool isWeaponWheelOpen;
extern bool isGamePaused;
extern bool isWeaponMenuOpen;

extern int currentWeapon;
extern std::vector<Object> bullets;
extern std::vector<Object> enemies;
extern Object player;

void InitGame();
void ResetGame();
void UpdateScore(int amount);

void UpdatePlayer(float dt);
void UpdateBullets(float dt);
void UpdateEnemies(float dt);
void CheckCollisions();
void DrawInterface();
void DrawEntities();

#define CLR_MAIN (Color){ 0, 255, 245, 255 }
#define CLR_SUB  (Color){ 255, 0, 110, 255 }
#define CLR_BG   (Color){ 5, 5, 15, 255 }

#endif