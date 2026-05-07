#include "raylib.h"
#include <direct.h>
#include <math.h>
#include <vector>
#include "../include/imgui.h"
#include "../include/rlImGui-main/rlImGui.h"
#include "../UI/player_setting/cs_player_setting_UI.h"
#include "../UI/player_setting/cs_click.h"
#include "../UI/player_setting/cs_click_effect.h"
#include "../Animation/UI_Animation/Player_setting/cs_player_setting_animation.h"
#include "../data/cs_data.h"
#include "../player/cs_move.h"
#include "../player/cs_attack.h"
#include "../player/cs_player_character.h"
#include "../AI/Cs_AI_Manager/cs_ai_manager.h"
#include "../AI/Cs_AI_Play/cs_ai_play.h"
#include "../AI/Cs_AI_Play/cs_ai_character.h"
#include "../physics/cs_physics.h"
#include "../particles/cs_explosion.h"
#include "../Weapon_UI/cs_weapon_ui.h"
#include "../Weapon_UI/cs_game_freezes.h"
#include "../particles/cs_flame.h"
#include "../Animation/cs_move_Animation.h"
#include "../particles/cs_spark.h"
#include "../particles/cs_performance.h"
#include "../particles/cs_performance.h"
#include "../Score/cs_score.h"
#include "../background/cs_background_Load.h"
#include "../weapon/Weapon_Use/cs_Rotary_Use.h"
#include "../weapon/Weapon/cs_rpg.h"
#include "../weapon/Weapon/cs_shotgun.h"
#include "../Camera/Cinematic_Camera/cs_Camera.h"
#include "../Heart/AI_Heart/AI_Heart_Manager/Heart_Manager.h"
#include "../Heart/Player_Heart/cs_player_heart_main.h"
#include "../Animation/Loading_Screen_Animation/cs_Screen.h"
#include "../Animation/Loading_Screen_Animation/cs_shaking.h"
#include "../Animation/Loading_Screen_Animation/cs_decoration.h"
#include "../Animation/Loading_Screen_Animation/cs_Create_TV.h"
#include "../Animation/Loading_Screen_Animation/cs_text_falling.h"
#include "../Boss/Boss_Manager/cs_boss_manager.h"
#include "../Boss/cs_earth_shaking.h"

enum LoadingSubStep {
    SUB_STEP_TEXT_FALLING,
    SUB_STEP_DECORATION_TV
};

static LoadingSubStep currentLoadingSubStep = SUB_STEP_TEXT_FALLING;
Camera2D screenCamera = { 0 };
int frame_counter = 0; 
bool bossTriggered = false;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cyber_Strack");
    
    _chdir(GetApplicationDirectory()); 
    _chdir("../");

    rlImGuiSetup(true);

    LoadGameBackground();
    InitPlayerCharacter();
    InitAICharacter();
    InitPlayerHeart();
    InitGame();
    InitScore();
    InitLoadingScreen(); 
    InitTextFalling();
    InitCreateTV();
    InitDecoration();

    SetTargetFPS(FPS);

    screenCamera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        frame_counter++; 

        UpdateScreenShake(dt);
        UpdateEarthShake(dt);
        UpdateSettingsInput();           
        UpdateSettingAnimation(dt);      

        if (IsKeyPressed(KEY_TAB)) {
            isWeaponMenuOpen = !isWeaponMenuOpen;
        }

        if (state == TITLE) {
            if (IsKeyPressed(KEY_ENTER)) {
                state = LOADING; 
                currentLoadingSubStep = SUB_STEP_TEXT_FALLING;
                InitTextFalling();
            }
        } 
        else if (state == LOADING) {
            if (currentLoadingSubStep == SUB_STEP_TEXT_FALLING) {
                UpdateTextFalling(dt);
                if (IsTextFallingFinished()) {
                    currentLoadingSubStep = SUB_STEP_DECORATION_TV;
                    InitCreateTV();
                    InitDecoration();
                }
            } 
            else if (currentLoadingSubStep == SUB_STEP_DECORATION_TV) {
                UpdateCreateTV(dt);
                UpdateDecoration(dt);
                
                if (IsTVAnimationFinished()) {
                    state = PLAYING;
                }
            }
        }
        else if (state == PLAYING) {
            UpdateGameFreeze();

            if (!isGamePaused && !isWeaponMenuOpen) {
                UpdatePlayerMove(dt);
                UpdatePlayerHeart(); 

                UpdateShotgun(dt);
                UpdatePlayerAttack(dt); 
                
                if (score >= 1000 && !bossTriggered) {
                    bossTriggered = true;
                    enemies.clear();
                    InitBoss();
                    StartEarthShake(1.5f, 10.0f);
                }

                if (bossTriggered) {
                    UpdateBoss(dt, player.pos);
                    if (gameBoss.state == BOSS_APPEARING) {
                        StartEarthShake(0.1f, 3.0f);
                    }
                } else {
                    UpdateAIManager(dt, player.pos.x, player.pos.y);
                    ProcessAIBehavior(dt); 
                }

                UpdatePhysics(dt);
                UpdateExplosions(dt);
                UpdateSparks(dt);
                UpdateFlames(dt);
                UpdateMoveAnimation(dt);
                UpdateScoreSystem(dt);

                for (int i = 0; i < (int)bullets.size(); i++) {
                    bullets[i].pos.x += bullets[i].vel.x * dt;
                    bullets[i].pos.y += bullets[i].vel.y * dt;
                    if (bullets[i].pos.y < -100 || bullets[i].pos.y > SCREEN_HEIGHT + 100 || 
                        bullets[i].pos.x < -100 || bullets[i].pos.x > SCREEN_WIDTH + 100) {
                        bullets.erase(bullets.begin() + i);
                        i--;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(CLR_BG);

        if (state == TITLE) {
            DrawText("PRESS ENTER", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, 20, RAYWHITE);
        } 
        else if (state == LOADING) {
            if (currentLoadingSubStep == SUB_STEP_TEXT_FALLING) {
                rlImGuiBegin();
                DrawTextFalling();
                rlImGuiEnd();
            } 
            else if (currentLoadingSubStep == SUB_STEP_DECORATION_TV) {
                DrawCreateTV();
                DrawDecoration(); 
            }
        }
        else if (state == PLAYING) {
            Vector2 shake = GetEarthShakeOffset();
            screenCamera.offset.x = GetCameraOffset().x + shake.x;
            screenCamera.offset.y = GetCameraOffset().y + shake.y;
            
            BeginMode2D(screenCamera);
                DrawGameBackground();
                DrawFlames();
                DrawPlayerCharacter();
                
                if (bossTriggered) {
                    DrawBoss();
                } else {
                    DrawAIManager(); 
                }

                DrawRPG();
                DrawShotgun();

                for (const auto& b : bullets) {
                    DrawRectangleV(b.pos, b.size, YELLOW);
                }

                RenderRotaryEffect(); 

                BeginBlendMode(BLEND_ADDITIVE);
                    DrawExplosions();
                    DrawSparks();
                EndBlendMode();
            EndMode2D();

            DrawScore();
            DrawWeaponUI();

            rlImGuiBegin(); 
            if (!isGamePaused && !isWeaponMenuOpen) {
                UpdateAndDrawParticles(dt);
                if (!bossTriggered) ProcessAiHeartSystem(enemies);
                DrawPlayerHeart(); 
            }
            DrawPlayerSettingUI();                                          
            rlImGuiEnd(); 
        }

        DrawSettingsOverlay();           

        EndDrawing();
    }

    rlImGuiShutdown();
    UnloadPlayerCharacter();
    UnloadAICharacter();
    UnloadGameBackground();
    CloseWindow();
    
    return 0;
}