#include "cs_Create_TV.h"
#include "raylib.h"
#include "rlgl.h"
#include <math.h>

static float animTimer = 0.0f;
static bool isFinished = false;
static Camera3D camera = { 0 };
static float tvScale = 0.0f;
static float fallY = 50.0f;
static float rotationX = -35.0f;

void InitCreateTV() {
    animTimer = 0.0f;
    isFinished = false;
    tvScale = 0.0f;
    fallY = 50.0f;
    rotationX = -35.0f;
    
    camera.position = (Vector3){ 0.0f, 0.5f, 30.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 25.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void UpdateCreateTV(float dt) {
    animTimer += dt;
    if (animTimer < 0.6f) return;

    float t = animTimer - 0.6f;

    if (t < 0.9f) {
        float ease = 1.0f - powf(1.0f - (t / 0.9f), 4);
        fallY = 50.0f * (1.0f - ease);
        rotationX = -35.0f * (1.0f - ease);
        tvScale = ease;
    } else {
        fallY = 0.0f;
        rotationX = 0.0f;
        tvScale = 1.0f;
    }

    if (animTimer > 2.0f) {
        float zoomT = animTimer - 2.0f;
        float duration = 2.0f;
        float progress = zoomT / duration;

        float zSpeed = powf(progress, 3.0f) * 250.0f;
        camera.position.z = 30.0f - (zoomT * 10.0f) - zSpeed;
        
        camera.fovy = 25.0f + powf(progress, 2.0f) * 120.0f; 
        camera.target.y = sinf(animTimer * 15.0f) * (0.1f + progress * 0.5f);

        if (zoomT >= duration || camera.position.z < -100.0f) isFinished = true;
    }
}

void DrawCreateTV() {
    if (animTimer < 0.6f) return;

    BeginMode3D(camera);

    float s = tvScale;
    
    rlPushMatrix();
    rlTranslatef(0, fallY, 0);
    rlRotatef(rotationX, 1, 0, 0);

    Color bDark = { 10, 10, 13, 255 };
    Color bMid = { 30, 30, 35, 255 };
    Color bLight = { 140, 140, 160, 255 };
    Color sDeep = { 2, 4, 8, 255 };
    Color spec = { 255, 255, 255, 80 };
    Color bRef = { 0, 255, 245, 40 };

    DrawCube((Vector3){0, 0, -1.3f * s}, 5.5f * s, 4.5f * s, 4.0f * s, bMid);
    DrawCubeWires((Vector3){0, 0, -1.3f * s}, 5.52f * s, 4.52f * s, 4.02f * s, bLight);
    DrawCube((Vector3){0, 0, -2.1f * s}, 5.1f * s, 4.1f * s, 4.0f * s, bDark);
    DrawCube((Vector3){0, 0, 0.7f * s}, 5.8f * s, 4.8f * s, 0.5f * s, bDark);
    DrawCubeWires((Vector3){0, 0, 0.7f * s}, 5.83f * s, 4.83f * s, 0.53f * s, bLight);

    Vector3 sp = { -0.7f * s, 0, 0.75f * s };
    DrawCube(sp, 4.1f * s, 3.3f * s, 0.1f * s, sDeep);
    DrawCube(sp, 4.12f * s, 3.32f * s, 0.12f * s, bRef);
    DrawCube((Vector3){-1.7f * s, 1.1f * s, 0.86f * s}, 1.3f * s, 0.9f * s, 0.01f * s, spec);

    Vector3 k1 = { 2.4f * s, 1.4f * s, 1.1f * s };
    Vector3 k2 = { 2.4f * s, 0.2f * s, 1.1f * s };
    DrawCylinderEx(k1, (Vector3){2.4f * s, 1.4f * s, 1.42f * s}, 0.4f * s, 0.38f * s, 16, bDark);
    DrawSphere((Vector3){2.4f * s, 1.4f * s, 1.42f * s}, 0.38f * s, bLight);
    DrawCylinderEx(k2, (Vector3){2.4f * s, 0.2f * s, 1.42f * s}, 0.4f * s, 0.38f * s, 16, bDark);
    DrawSphere((Vector3){2.4f * s, 0.2f * s, 1.42f * s}, 0.38f * s, bLight);

    Vector3 aB = { 0, 2.45f * s, -1.05f * s };
    DrawLine3D(aB, (Vector3){-3.3f * s, 5.9f * s, -0.55f * s}, bLight);
    DrawLine3D(aB, (Vector3){ 3.3f * s, 5.9f * s, -0.55f * s}, bLight);

    if (animTimer > 2.0f) {
        float speed = (animTimer - 2.0f) * 120.0f;
        Color gridCol = { 0, 255, 245, 150 };
        
        for (int z = 0; z < 40; z++) {
            float zPos = 10.0f - (z * 6.0f) + fmodf(speed, 6.0f);
            float alphaFade = 1.0f - (z / 40.0f);
            Color currentCol = { gridCol.r, gridCol.g, gridCol.b, (unsigned char)(gridCol.a * alphaFade) };

            for (int y = -8; y <= 8; y += 16) {
                DrawLine3D((Vector3){-40, (float)y, zPos}, (Vector3){40, (float)y, zPos}, currentCol);
            }
            for (int x = -40; x <= 40; x += 10) {
                DrawLine3D((Vector3){(float)x, -8, 10}, (Vector3){(float)x, -8, -200}, gridCol);
                DrawLine3D((Vector3){(float)x,  8, 10}, (Vector3){(float)x,  8, -200}, gridCol);
            }
        }

        for (int i = 0; i < 60; i++) {
            float pz = 10.0f - fmodf(speed * (1.2f + i * 0.05f), 250.0f);
            float px = sinf(i * 0.3f) * 30.0f;
            float py = cosf(i * 0.7f) * 15.0f;
            DrawCube((Vector3){px, py, pz}, 0.3f, 0.3f, 5.0f, {0, 255, 245, 220});
        }
        
        for (int i = 0; i < 20; i++) {
            float hz = 5.0f - (i * 15.0f) + fmodf(speed * 0.5f, 15.0f);
            DrawCubeWires((Vector3){0, 0, hz}, 15.0f + i, 10.0f + i, 1.0f, {0, 255, 245, (unsigned char)(50 - i)});
        }
    }

    rlPopMatrix();
    EndMode3D();
}

bool IsTVAnimationFinished() {
    return isFinished;
}