#include "cs_Create_TV.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>

const char* tvVs =
    "#version 330\n"
    "in vec3 vertexPosition;\n"
    "in vec2 vertexTexCoord;\n"
    "out vec2 fragTexCoord;\n"
    "uniform mat4 mvp;\n"
    "void main() {\n"
    "    fragTexCoord = vertexTexCoord;\n"
    "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
    "}\n";

const char* tvFs =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D screenTexture;\n"
    "uniform float brightness;\n"
    "void main() {\n"
    "    vec4 tex = texture(screenTexture, fragTexCoord);\n"
    "    vec3 color = tex.rgb * brightness;\n"
    "    color = pow(color, vec3(1.0/1.1));\n"
    "    float dist = distance(fragTexCoord, vec2(0.5));\n"
    "    color *= (1.0 - dist * 0.3);\n"
    "    finalColor = vec4(color, 1.0);\n"
    "}\n";

struct WorldObj { Vector3 pos; float scale; Color color; int type; };

static RenderTexture2D tvTarget;
static Shader tvShader;
static int locBright;
static Camera3D tvInternalCam = { 0 };
static Camera3D worldCam = { 0 };
static std::vector<WorldObj> ruralObjects;
static float animTime = 0.0f;
static bool resourcesLoaded = false;
static bool isFinished = false;

void InitCreateTV() {
    animTime = 0.0f;
    isFinished = false;

    if (!resourcesLoaded) {
        tvTarget = LoadRenderTexture(1280, 720);
        tvShader = LoadShaderFromMemory(tvVs, tvFs);
        locBright = GetShaderLocation(tvShader, "brightness");
        resourcesLoaded = true;
    }

    tvInternalCam.position = (Vector3){ 15.0f, 10.0f, 15.0f };
    tvInternalCam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    tvInternalCam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    tvInternalCam.fovy = 45.0f;
    tvInternalCam.projection = CAMERA_PERSPECTIVE;

    worldCam.position = (Vector3){ 0.2f, 3.5f, 1.5f };
    worldCam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    worldCam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    worldCam.fovy = 30.0f;
    worldCam.projection = CAMERA_PERSPECTIVE;

    ruralObjects.clear();
    for (int i = 0; i < 40; i++) {
        ruralObjects.push_back({
            (Vector3){ (float)(rand()%40-20), -1.0f, (float)(rand()%40-20) },
            (float)(rand()%100)/50.0f + 1.0f,
            (Color){ (unsigned char)(30+rand()%30), (unsigned char)(100+rand()%50), 40, 255 },
            rand()%2
        });
    }
}

static void DrawRuralContent() {
    BeginTextureMode(tvTarget);
    ClearBackground((Color){ 180, 210, 230, 255 });
    BeginMode3D(tvInternalCam);
    
    DrawPlane((Vector3){0, -1, 0}, (Vector2){100, 100}, (Color){70, 90, 60, 255});
    for(const auto& obj : ruralObjects) {
        if(obj.type == 0) {
            DrawCylinderEx(obj.pos, Vector3Add(obj.pos, (Vector3){0, obj.scale, 0}), 0.2f, 0.1f, 8, (Color){80, 60, 50, 255});
            DrawSphere(Vector3Add(obj.pos, (Vector3){0, obj.scale, 0}), obj.scale*0.6f, obj.color);
        } else {
            DrawCube(obj.pos, 2.0f, 1.5f, 2.0f, (Color){180, 160, 140, 255});
            DrawCylinderEx(Vector3Add(obj.pos, (Vector3){0, 0.75f, 0}), Vector3Add(obj.pos, (Vector3){0, 1.5f, 0}), 1.5f, 0.1f, 4, (Color){150, 50, 40, 255});
        }
    }
    
    EndMode3D();
    EndTextureMode();
}

void UpdateCreateTV(float dt) {
    if (isFinished) return;
    animTime += dt;

    float t = fminf(animTime / 4.0f, 1.0f);
    float ease = 1.0f - powf(1.0f - t, 3.0f);

    worldCam.position.z = Lerp(1.5f, 15.0f, ease);
    worldCam.position.y = Lerp(3.5f, 6.0f, ease);
    worldCam.fovy = Lerp(30.0f, 45.0f, ease);

    tvInternalCam.position.x = 15.0f + sinf(animTime * 0.2f) * 5.0f;

    if (animTime >= 5.0f) isFinished = true;
}

void DrawCreateTV() {
    if (isFinished) return;

    DrawRuralContent();

    BeginMode3D(worldCam);

    DrawCube((Vector3){0, 0, -0.6f}, 4.8f, 3.6f, 1.2f, (Color){30, 32, 35, 255});
    DrawCube((Vector3){0, 0, -0.1f}, 5.0f, 3.8f, 0.2f, (Color){20, 20, 22, 255});
    
    float dialX = 2.0f;
    DrawCylinderEx((Vector3){dialX, 0.8f, 0.0f}, (Vector3){dialX, 0.8f, 0.2f}, 0.2f, 0.2f, 12, (Color){50, 50, 55, 255});
    DrawCylinderEx((Vector3){dialX, 0.2f, 0.0f}, (Vector3){dialX, 0.2f, 0.2f}, 0.2f, 0.2f, 12, (Color){50, 50, 55, 255});

    float brightValue = (animTime < 0.5f) ? animTime * 3.0f : 1.5f;
    BeginShaderMode(tvShader);
    SetShaderValue(tvShader, locBright, &brightValue, SHADER_UNIFORM_FLOAT);
    
    rlSetTexture(tvTarget.texture.id);
    rlBegin(RL_QUADS);
        rlColor4ub(255, 255, 255, 255);
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-1.9f, -1.4f, 0.05f);
        rlTexCoord2f(1.0f, 1.0f); rlVertex3f(1.1f, -1.4f, 0.05f);
        rlTexCoord2f(1.0f, 0.0f); rlVertex3f(1.1f, 1.4f, 0.05f);
        rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-1.9f, 1.4f, 0.05f);
    rlEnd();
    rlSetTexture(0);
    
    EndShaderMode();

    EndMode3D();

    if (animTime > 4.0f) {
        float alpha = (animTime - 4.0f);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, fminf(alpha, 1.0f)));
    }
}

bool IsTVAnimationFinished() {
    return isFinished;
}