#include "cs_player_setting_animation.h"
#include "../../../UI/player_setting/cs_click.h"
#include <algorithm>
#include <cmath>

float settingAnimPercent = 0.0f;

void UpdateSettingAnimation(float dt) {
    float target = isSettingsOpen ? 1.0f : 0.0f;
    
    float speed = isSettingsOpen ? 2.5f : 4.0f;

    if (settingAnimPercent < target) {
        settingAnimPercent = std::min(target, settingAnimPercent + speed * dt);
    } else if (settingAnimPercent > target) {
        settingAnimPercent = std::max(target, settingAnimPercent - speed * dt);
    }
}

float GetEaseOutBack(float x) {
    const float c1 = 1.20158f; 
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(x - 1.0f, 3.0f) + c1 * std::pow(x - 1.0f, 2.0f);
}