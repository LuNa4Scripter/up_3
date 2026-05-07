#include "cs_data.h"
#include "../AI/Cs_AI_Manager/cs_ai_manager.h"

GameState state = TITLE;
int score = 0;
int hiScore = 0;
float timer = 0.0f;
float diff = 1.0f;
bool isWeaponWheelOpen = false;
bool isGamePaused = false; 
bool isWeaponMenuOpen = false;

int currentWeapon = 0; 

std::vector<Object> bullets;
std::vector<Object> enemies; 
Object player = { 0 };

void InitGame() {
    player.pos = (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT - 100.0f };
    player.size = (Vector2){ 40, 40 };
    player.vel = (Vector2){ 0, 0 }; 
    player.hp = 3;
    player.cooldown = 0; 
    player.active = true;
    
    bullets.reserve(100);
    enemies.reserve(50);
}

void ResetGame() {
    score = 0;
    timer = 0;
    diff = 1.0f;
    currentWeapon = 0; 
    bullets.clear();
    enemies.clear();
    player.pos = (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT - 100.0f };
    player.vel = (Vector2){ 0, 0 };
    player.hp = 3;
    player.active = true;
    isGamePaused = false;
}

void UpdateScore(int amount) {
    score += amount;
    if (score > hiScore) hiScore = score;
}

void UpdatePlayer(float dt) {
    float moveSpeed = 450.0f;
    if (IsKeyDown(KEY_W) && player.pos.y > 0) player.pos.y -= moveSpeed * dt;
    if (IsKeyDown(KEY_S) && player.pos.y < SCREEN_HEIGHT - player.size.y) player.pos.y += moveSpeed * dt;
    if (IsKeyDown(KEY_A) && player.pos.x > 0) player.pos.x -= moveSpeed * dt;
    if (IsKeyDown(KEY_D) && player.pos.x < SCREEN_WIDTH - player.size.x) player.pos.x += moveSpeed * dt;

    player.cooldown += dt;
    if (IsKeyDown(KEY_SPACE) && player.cooldown > 0.15f) {
        bullets.push_back({{player.pos.x + 18, player.pos.y}, {0, -600}, {4, 12}, 1, 0, 0, 0, true});
        player.cooldown = 0;
    }
}

void UpdateBullets(float dt) {
    for (int i = 0; i < (int)bullets.size(); i++) {
        bullets[i].pos.y += bullets[i].vel.y * dt;
        if (bullets[i].pos.y < -20) {
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }
}

void UpdateEnemies(float dt) {
    timer += dt;
    if (timer > (0.6f / diff)) {
        float x = (float)GetRandomValue(0, SCREEN_WIDTH - 40);
        
        Object e = { 0 };
        e.pos = { x, -50.0f };
        e.size = { 40, 40 };
        e.spd = (float)GetRandomValue(150, 300) * diff;
        e.active = true;
        e.is_dead = false;
        
        enemies.push_back(e);
        
        timer = 0;
        diff += 0.003f;
    }

    for (int i = 0; i < (int)enemies.size(); i++) {
        enemies[i].pos.y += enemies[i].spd * dt;
        
        if (enemies[i].pos.y > SCREEN_HEIGHT + 50) {
            enemies.erase(enemies.begin() + i);
            i--;
        }
    }
}

void CheckCollisions() {
    for (int i = 0; i < (int)enemies.size(); i++) {
        if (CheckCollisionRecs({player.pos.x, player.pos.y, player.size.x, player.size.y}, 
                               {enemies[i].pos.x, enemies[i].pos.y, enemies[i].size.x, enemies[i].size.y})) {
            state = GAMEOVER;
        }

        for (int j = 0; j < (int)bullets.size(); j++) {
            if (CheckCollisionRecs({bullets[j].pos.x, bullets[j].pos.y, bullets[j].size.x, bullets[j].size.y},
                                   {enemies[i].pos.x, enemies[i].pos.y, enemies[i].size.x, enemies[i].size.y})) {
                enemies.erase(enemies.begin() + i);
                bullets.erase(bullets.begin() + j);
                UpdateScore(100);
                i--;
                break;
            }
        }
    }
}

void DrawInterface() {
    if (state == TITLE) {
        DrawText("CYBER SKY", SCREEN_WIDTH/2 - MeasureText("CYBER SKY", 50)/2, 200, 50, CLR_MAIN);
        DrawText("PRESS ENTER TO START", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER TO START", 20)/2, 300, 20, RAYWHITE);
    } else if (state == PLAYING) {
        DrawText(TextFormat("SCORE %05i", score), 20, 20, 20, CLR_MAIN);
        DrawText(TextFormat("HI %05i", hiScore), 20, 45, 15, GRAY);
    } else if (state == GAMEOVER) {
        DrawText("SYSTEM HALTED", SCREEN_WIDTH/2 - MeasureText("SYSTEM HALTED", 40)/2, 200, 40, CLR_SUB);
        DrawText(TextFormat("FINAL SCORE: %i", score), SCREEN_WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %i", score), 20)/2, 260, 20, RAYWHITE);
        DrawText("PRESS ENTER TO RETRY", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER TO RETRY", 15)/2, 350, 15, GRAY);
    }
}

void DrawEntities() {
    if (state == PLAYING) {
        DrawRectangleV(player.pos, player.size, CLR_MAIN);
        for (auto& b : bullets) DrawRectangleV(b.pos, b.size, YELLOW);
        for (auto& e : enemies) DrawRectangleV(e.pos, e.size, CLR_SUB);
    }
}