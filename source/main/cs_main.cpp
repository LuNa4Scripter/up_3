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

class UIManager {
public:
    void InitializeSystem();
    void Update(float dt);
    void Render();
    void ShutdownSystem();
};

extern UIManager g_SystemHUD; 
extern void InitDecoration();
extern void UpdateDecoration(float dt);
extern void DrawDecoration();

Camera2D screenCamera = { 0 };
int frame_counter = 0; 

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
    
    // [추가] 다이제틱 UI 초기화 (cs_decoration.cpp에 정의한 함수 호출)
    InitDecoration(); 

    SetTargetFPS(FPS);

    screenCamera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        frame_counter++; 

        UpdateScreenShake(dt);
        UpdateSettingsInput();           
        UpdateSettingAnimation(dt);      

        // [추가] 다이제틱 UI 업데이트
        UpdateDecoration(dt);

        if (IsKeyPressed(KEY_TAB)) {
            isWeaponMenuOpen = !isWeaponMenuOpen;
        }

        if (state == TITLE) {
            if (IsKeyPressed(KEY_ENTER)) {
                state = LOADING; 
                InitLoadingScreen();
            }
        } 
        else if (state == LOADING) {
            UpdateLoadingScreen(dt); 
            
            if (IsTVAnimationFinished()) {
                state = PLAYING;
            }
        }
        else if (state == PLAYING) {
            UpdateGameFreeze();

            if (!isGamePaused && !isWeaponMenuOpen) {
                UpdatePlayerMove(dt);
                UpdatePlayerHeart(); 

                UpdateShotgun(dt);
                UpdatePlayerAttack(dt); 
                
                UpdateAIManager(dt, player.pos.x, player.pos.y);
                ProcessAIBehavior(dt); 

                UpdatePhysics(dt);
                UpdateExplosions(dt);
                UpdateSparks(dt);
                UpdateFlames(dt);
                UpdateMoveAnimation(dt);
                UpdateScoreSystem(dt);

                for (int i = 0; i < (int)bullets.size(); i++) {
                    bullets[i].pos.y += bullets[i].vel.y * dt;
                    if (bullets[i].pos.y < -50 || bullets[i].pos.y > SCREEN_HEIGHT + 50 || 
                        bullets[i].pos.x < -50 || bullets[i].pos.x > SCREEN_WIDTH + 50) {
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
            DrawLoadingScreen(); 
        }
        else if (state == PLAYING) {
            screenCamera.offset = GetCameraOffset();
            
            BeginMode2D(screenCamera);
                DrawGameBackground();
                DrawFlames();
                DrawPlayerCharacter();
                DrawAIManager(); 
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
            // [추가] 다이제틱 UI 렌더링 (rlImGui 블록 내부에서 호출해야 함)
            DrawDecoration();

            if (!isGamePaused && !isWeaponMenuOpen) {
                UpdateAndDrawParticles(dt);
                ProcessAiHeartSystem(enemies);
                DrawPlayerHeart(); 
            }
            DrawPlayerSettingUI();                                   
            rlImGuiEnd(); 
        }

        DrawSettingsOverlay();           

        EndDrawing();
    }

    // [추가] 시스템 종료 처리
    g_SystemHUD.ShutdownSystem();

    rlImGuiShutdown();
    UnloadPlayerCharacter();
    UnloadAICharacter();
    UnloadGameBackground();
    CloseWindow();
    
    return 0;
}