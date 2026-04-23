#ifndef CS_POSITION_H
#define CS_POSITION_H

#include "../../include/imgui.h"

struct UiPos {
    float x;
    float y;
    float width;
    float padding;
};

void SetAiUiPosition(UiPos& pos);

#endif