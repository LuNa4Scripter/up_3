#include "cs_earth_shaking.h"
#include <stdlib.h>

static float shakeDuration = 0.0f;
static float shakeIntensity = 0.0f;
static Vector2 shakeOffset = { 0, 0 };

void StartEarthShake(float duration, float intensity) {
    shakeDuration = duration;
    shakeIntensity = intensity;
}

void UpdateEarthShake(float dt) {
    if (shakeDuration > 0) {
        shakeDuration -= dt;
        shakeOffset.x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeIntensity;
        shakeOffset.y = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeIntensity;

        if (shakeDuration <= 0) {
            shakeOffset = { 0, 0 };
        }
    }
}

Vector2 GetEarthShakeOffset() {
    return shakeOffset;
}