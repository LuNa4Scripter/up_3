#include "cs_Camera.h"
#include <math.h>

float shakeIntensity = 0.0f;
float shakeDuration = 0.0f;
Vector2 cameraOffset = { 0, 0 };

void BeginScreenShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeDuration = duration;
}

void UpdateScreenShake(float dt) {
    if (shakeDuration > 0) {
        shakeDuration -= dt;
        cameraOffset.x = (float)GetRandomValue(-(int)shakeIntensity, (int)shakeIntensity);
        cameraOffset.y = (float)GetRandomValue(-(int)shakeIntensity, (int)shakeIntensity);
    } else {
        cameraOffset = (Vector2){ 0, 0 };
    }
}

Vector2 GetCameraOffset() {
    return cameraOffset;
}