#include "raylib.h"
#include "rlgl.h"
#include "../../include/imgui.h"
#include "../../include/rlImGui-main/rlImGui.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <cmath>
#include <algorithm>
#include <cstdint>

inline float InternalGetLinearInterp(float start, float end, float t) {
    return start + t * (end - start);
}

class UIManager; 
extern UIManager g_SystemHUD;

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1280
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 720
#endif

enum class UIElementType {
    BASE_CONTAINER,
    DATA_PANEL,
    STATUS_MONITOR,
    SYSTEM_GRAPH,
    TACTICAL_FRAME,
    VIRTUAL_SENSOR,
    HYPER_LINK
};

enum class UIElementState {
    INACTIVE,
    INITIALIZING,
    BOOTING,
    ACTIVE,
    WARNING,
    CRITICAL,
    SHUTTING_DOWN,
    TERMINATED
};

enum class UILayerDepth {
    BACKGROUND_STATIC,
    MIDGROUND_DYNAMIC,
    FOREGROUND_HUD,
    OVERLAY_GLITCH
};

struct UIStyleProfile {
    ImVec4 primaryColor;
    ImVec4 secondaryColor;
    ImVec4 accentColor;
    ImVec4 warningColor;
    float borderSize;
    float rounding;
    float padding;
    float shadowIntensity;
};

class UIElement {
protected:
    std::string elementID;
    uint32_t elementHash;
    UIElementType elementType;
    UIElementState currentState;
    UILayerDepth layerDepth;
    UIStyleProfile style;
    
    ImVec2 basePosition;
    ImVec2 currentPosition;
    ImVec2 elementSize;
    ImVec2 targetSize;
    
    float masterAlpha;
    float targetAlpha;
    float currentScale;
    float targetScale;
    float rotationAngle;
    
    bool isVisible;
    bool isInteractable;
    bool isInitialized;
    bool isFocusLocked;

    double creationTimestamp;
    float lifeTime;
    float internalTimer;
    float stateTimer;

    float InternalCalculateLerp(float start, float end, float t) {
        return start + t * (end - start);
    }

    float InternalGetNormalizedTime(float current, float duration) {
        if (duration <= 0.0f) return 1.0f;
        float t = current / duration;
        return (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
    }

    void InternalUpdateTimers(float dt) {
        internalTimer += dt;
        lifeTime += dt;
        stateTimer += dt;
    }

    virtual void OnStateChanged(UIElementState oldState, UIElementState newState) {
        stateTimer = 0.0f;
    }

public:
    UIElement(std::string id, UIElementType type, ImVec2 pos, ImVec2 size)
        : elementID(id), elementType(type), basePosition(pos), elementSize(size) {
        currentState = UIElementState::INACTIVE;
        layerDepth = UILayerDepth::MIDGROUND_DYNAMIC;
        currentPosition = pos;
        targetSize = size;
        masterAlpha = 0.0f;
        targetAlpha = 1.0f;
        currentScale = 0.8f;
        targetScale = 1.0f;
        rotationAngle = 0.0f;
        isVisible = true;
        isInteractable = true;
        isInitialized = false;
        isFocusLocked = false;
        lifeTime = 0.0f;
        internalTimer = 0.0f;
        stateTimer = 0.0f;
        creationTimestamp = GetTime();

        style.primaryColor = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);
        style.secondaryColor = ImVec4(0.0f, 0.2f, 0.3f, 0.8f);
        style.accentColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.warningColor = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
        style.borderSize = 1.0f;
        style.rounding = 0.0f;
    }

    virtual ~UIElement() {}

    virtual void PreUpdate(float deltaTime) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void LateUpdate(float deltaTime) = 0;
    virtual void Render(ImDrawList* drawList) = 0;

    void SetState(UIElementState newState) {
        if (currentState != newState) {
            UIElementState old = currentState;
            currentState = newState;
            OnStateChanged(old, newState);
        }
    }

    UIElementState GetState() const { return currentState; }
    void SetLayer(UILayerDepth depth) { layerDepth = depth; }
    UILayerDepth GetLayer() const { return layerDepth; }
    void SetBasePosition(float x, float y) { basePosition.x = x; basePosition.y = y; }
    ImVec2 GetBasePosition() const { return basePosition; }
    void SetElementSize(float w, float h) { elementSize.x = w; elementSize.y = h; }
    ImVec2 GetElementSize() const { return elementSize; }
    std::string GetID() const { return elementID; }
    bool IsActive() const { return isVisible; }
};

class UIManager {
private:
    std::vector<std::unique_ptr<UIElement>> uiElementRegistry;
    std::map<std::string, UIElement*> elementLookup;
    
    float globalSystemTime;
    float masterOpacity;
    float systemBootProgress;
    bool systemInitialized;
    bool isRenderingEnabled;

    void InternalSortByLayer() {
        std::sort(uiElementRegistry.begin(), uiElementRegistry.end(), 
            [](const std::unique_ptr<UIElement>& a, const std::unique_ptr<UIElement>& b) {
            if (a->GetLayer() != b->GetLayer()) {
                return static_cast<int>(a->GetLayer()) < static_cast<int>(b->GetLayer());
            }
            return a->GetID() < b->GetID();
        });
    }

    void InternalValidateRegistry() {
        if (!systemInitialized) return;
    }

    void InternalProcessDeferredActions() {
        auto it = std::remove_if(uiElementRegistry.begin(), uiElementRegistry.end(), 
            [](const std::unique_ptr<UIElement>& e) {
            return e->GetState() == UIElementState::TERMINATED;
        });
        uiElementRegistry.erase(it, uiElementRegistry.end());
    }

    void ExecutePreUpdatePipeline(float dt) {
        for (auto& element : uiElementRegistry) {
            if (element->IsActive()) {
                element->PreUpdate(dt);
            }
        }
    }

    void ExecuteUpdatePipeline(float dt) {
        for (auto& element : uiElementRegistry) {
            if (element->IsActive()) {
                element->Update(dt);
            }
        }
    }

    void ExecuteLateUpdatePipeline(float dt) {
        for (auto& element : uiElementRegistry) {
            if (element->IsActive()) {
                element->LateUpdate(dt);
            }
        }
    }

public:
    UIManager() {
        globalSystemTime = 0.0f;
        masterOpacity = 1.0f;
        systemBootProgress = 0.0f;
        systemInitialized = false;
        isRenderingEnabled = true;
    }

    void InitializeSystem() {
        if (!systemInitialized) {
            systemInitialized = true;
            globalSystemTime = 0.0f;
            systemBootProgress = 1.0f;
        }
    }

    void RegisterElement(std::unique_ptr<UIElement> element) {
        std::string id = element->GetID();
        elementLookup[id] = element.get();
        uiElementRegistry.push_back(std::move(element));
        InternalSortByLayer();
    }

    UIElement* FindElement(const std::string& id) {
        if (elementLookup.find(id) != elementLookup.end()) {
            return elementLookup[id];
        }
        return nullptr;
    }

    void UnregisterElement(const std::string& id) {
        if (elementLookup.find(id) != elementLookup.end()) {
            elementLookup[id]->SetState(UIElementState::TERMINATED);
            elementLookup.erase(id);
        }
    }

    void Update(float dt) {
        if (!systemInitialized) return;
        
        globalSystemTime += dt;
        InternalProcessDeferredActions();
        
        ExecutePreUpdatePipeline(dt);
        ExecuteUpdatePipeline(dt);
        ExecuteLateUpdatePipeline(dt);
    }

    void Render() {
        if (!systemInitialized || !isRenderingEnabled) return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        for (auto& element : uiElementRegistry) {
            if (element->IsActive()) {
                element->Render(drawList);
            }
        }
    }

    void ShutdownSystem() {
        uiElementRegistry.clear();
        elementLookup.clear();
        systemInitialized = false;
    }
};

class HUDDataPanel : public UIElement {
protected:
    float headerRatio;
    float footerRatio;
    float currentFillAmount;
    float targetFillAmount;
    
    void DrawPanelBackground(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax, ImU32 color) {
        dl->AddRectFilled(pMin, pMax, color, style.rounding);
    }

    void DrawPanelHeader(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax, ImU32 color) {
        float h = (pMax.y - pMin.y) * headerRatio;
        dl->AddRectFilled(pMin, {pMax.x, pMin.y + h}, color, style.rounding, ImDrawFlags_RoundCornersTop);
    }

    void DrawPanelBorders(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax, ImU32 color) {
        dl->AddRect(pMin, pMax, color, style.rounding, 0, style.borderSize);
    }

    void DrawStatusIndicators(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax, ImU32 color) {
        float size = 4.0f;
        float pad = 6.0f;
        dl->AddCircleFilled({pMax.x - pad, pMin.y + pad}, size, color);
    }

public:
    HUDDataPanel(std::string id, ImVec2 pos, ImVec2 size)
        : UIElement(id, UIElementType::DATA_PANEL, pos, size) {
        headerRatio = 0.15f;
        footerRatio = 0.1f;
        currentFillAmount = 0.0f;
        targetFillAmount = 1.0f;
        style.rounding = 2.0f;
    }

    void PreUpdate(float dt) override {
        InternalUpdateTimers(dt);
    }

    void Update(float dt) override {
        if (!isInitialized) {
            isInitialized = true;
            SetState(UIElementState::ACTIVE);
        }
        
        masterAlpha = InternalCalculateLerp(masterAlpha, targetAlpha, 5.0f * dt);
        currentScale = InternalCalculateLerp(currentScale, targetScale, 8.0f * dt);
    }

    void LateUpdate(float dt) override {
        currentPosition = basePosition;
    }

    void Render(ImDrawList* drawList) override {
        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};
        
        ImU32 colBg = ImColor(style.secondaryColor.x, style.secondaryColor.y, style.secondaryColor.z, style.secondaryColor.w * masterAlpha);
        ImU32 colHd = ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z, 0.4f * masterAlpha);
        ImU32 colBr = ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z, 0.8f * masterAlpha);

        DrawPanelBackground(drawList, pMin, pMax, colBg);
        DrawPanelHeader(drawList, pMin, pMax, colHd);
        DrawPanelBorders(drawList, pMin, pMax, colBr);
        DrawStatusIndicators(drawList, pMin, pMax, colBr);
        
        drawList->AddText(NULL, 14.0f, {pMin.x + 8, pMin.y + 4}, ImColor(1.0f, 1.0f, 1.0f, masterAlpha), elementID.c_str());
    }
};
struct SpringValue {
    float current;
    float velocity;
    float target;
    float stiffness;
    float damping;
    float mass;

    SpringValue() : current(0.0f), velocity(0.0f), target(0.0f), stiffness(150.0f), damping(15.0f), mass(1.0f) {}

    void Reset(float val) {
        current = val;
        target = val;
        velocity = 0.0f;
    }

    void Update(float dt) {
        float force = (target - current) * stiffness;
        float acceleration = (force / mass) - (damping * velocity);
        velocity += acceleration * dt;
        current += velocity * dt;
    }

    void SetTarget(float t) {
        target = t;
    }
};

struct UIInteractionState {
    bool isHovered;
    bool isPressed;
    bool isFocused;
    float hoverTransition;
    float pressTransition;
    Vector2 lastMousePos;
    Vector2 dragOffset;
};

class PhysicsUIElement : public HUDDataPanel {
protected:
    SpringValue springPosX;
    SpringValue springPosY;
    SpringValue springScale;
    SpringValue springAlpha;
    SpringValue springRotation;

    UIInteractionState interaction;
    Vector2 inertiaOffset;
    float inertiaIntensity;
    float responsiveness;

    float InternalGetDistance(Vector2 v1, ImVec2 v2) {
        return sqrtf(powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2));
    }

    bool InternalCheckPointBounds(Vector2 point, ImVec2 pMin, ImVec2 pMax) {
        return (point.x >= pMin.x && point.x <= pMax.x && point.y >= pMin.y && point.y <= pMax.y);
    }

    float InternalApplyExponentialDecay(float current, float target, float speed, float dt) {
        return target + (current - target) * expf(-speed * dt);
    }

    void InternalUpdatePhysics(float dt) {
        springPosX.Update(dt);
        springPosY.Update(dt);
        springScale.Update(dt);
        springAlpha.Update(dt);
        springRotation.Update(dt);
    }

    void InternalHandleInertia(float dt) {
        Vector2 mPos = GetMousePosition();
        float targetInertiaX = (mPos.x - SCREEN_WIDTH * 0.5f) * inertiaIntensity;
        float targetInertiaY = (mPos.y - SCREEN_HEIGHT * 0.5f) * inertiaIntensity;

        inertiaOffset.x = InternalApplyExponentialDecay(inertiaOffset.x, targetInertiaX, responsiveness, dt);
        inertiaOffset.y = InternalApplyExponentialDecay(inertiaOffset.y, targetInertiaY, responsiveness, dt);
    }

    void InternalProcessInteraction(float dt) {
        Vector2 mPos = GetMousePosition();
        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};

        bool currentlyHovered = InternalCheckPointBounds(mPos, pMin, pMax);

        if (currentlyHovered && isInteractable) {
            if (!interaction.isHovered) {
                interaction.isHovered = true;
                OnHoverEnter();
            }
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                interaction.isPressed = true;
                OnPressed();
            }
        } else {
            if (interaction.isHovered) {
                interaction.isHovered = false;
                OnHoverExit();
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            interaction.isPressed = false;
        }

        float targetHover = interaction.isHovered ? 1.0f : 0.0f;
        interaction.hoverTransition = InternalApplyExponentialDecay(interaction.hoverTransition, targetHover, 10.0f, dt);
        
        float targetPress = interaction.isPressed ? 1.0f : 0.0f;
        interaction.pressTransition = InternalApplyExponentialDecay(interaction.pressTransition, targetPress, 15.0f, dt);
    }

    virtual void OnHoverEnter() {
        springScale.SetTarget(1.05f);
        springAlpha.SetTarget(1.0f);
    }

    virtual void OnHoverExit() {
        springScale.SetTarget(1.0f);
        springAlpha.SetTarget(0.85f);
    }

    virtual void OnPressed() {
        springScale.SetTarget(0.95f);
    }

public:
    PhysicsUIElement(std::string id, ImVec2 pos, ImVec2 size)
        : HUDDataPanel(id, pos, size) {
        inertiaIntensity = 0.04f;
        responsiveness = 6.0f;
        inertiaOffset = {0.0f, 0.0f};
        
        interaction.isHovered = false;
        interaction.isPressed = false;
        interaction.isFocused = false;
        interaction.hoverTransition = 0.0f;
        interaction.pressTransition = 0.0f;

        springPosX.Reset(pos.x);
        springPosY.Reset(pos.y);
        springScale.Reset(0.8f);
        springScale.SetTarget(1.0f);
        springAlpha.Reset(0.0f);
        springAlpha.SetTarget(0.85f);
        springRotation.Reset(0.0f);
    }

    void PreUpdate(float dt) override {
        HUDDataPanel::PreUpdate(dt);
        InternalHandleInertia(dt);
        InternalProcessInteraction(dt);
    }

    void Update(float dt) override {
        float targetX = basePosition.x + inertiaOffset.x;
        float targetY = basePosition.y + inertiaOffset.y;

        springPosX.SetTarget(targetX);
        springPosY.SetTarget(targetY);

        InternalUpdatePhysics(dt);

        currentPosition.x = springPosX.current;
        currentPosition.y = springPosY.current;
        currentScale = springScale.current;
        masterAlpha = springAlpha.current;
        rotationAngle = springRotation.current;
    }

    void Render(ImDrawList* drawList) override {
        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};
        
        float pulse = sinf(internalTimer * 2.0f) * 0.05f;
        ImU32 colGlow = ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z, (0.2f + pulse + interaction.hoverTransition * 0.2f) * masterAlpha);
        
        for(int i = 1; i <= 3; ++i) {
            float expand = (float)i * 2.0f * interaction.hoverTransition;
            drawList->AddRect(
                {pMin.x - expand, pMin.y - expand}, 
                {pMax.x + expand, pMax.y + expand}, 
                colGlow, style.rounding, 0, 1.0f
            );
        }

        HUDDataPanel::Render(drawList);

        if (interaction.isPressed) {
            drawList->AddRect(pMin, pMax, ImColor(1.0f, 1.0f, 1.0f, 0.3f), style.rounding, 0, 2.0f);
        }
    }
};

class TacticalFrameElement : public PhysicsUIElement {
private:
    float scanlineOffset;
    std::vector<float> historyBuffer;

    void InternalUpdateHistory() {
        if (historyBuffer.size() > 20) historyBuffer.erase(historyBuffer.begin());
        historyBuffer.push_back((float)GetRandomValue(30, 90));
    }

    void DrawTacticalGrid(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax) {
        float step = 20.0f;
        ImU32 gridCol = ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z, 0.05f * masterAlpha);
        for (float x = pMin.x; x < pMax.x; x += step) dl->AddLine({x, pMin.y}, {x, pMax.y}, gridCol);
        for (float y = pMin.y; y < pMax.y; y += step) dl->AddLine({pMin.x, y}, {pMax.x, y}, gridCol);
    }

public:
    TacticalFrameElement(std::string id, ImVec2 pos, ImVec2 size)
        : PhysicsUIElement(id, pos, size) {
        scanlineOffset = 0.0f;
        inertiaIntensity = 0.07f;
        style.secondaryColor = ImVec4(0.01f, 0.03f, 0.05f, 0.9f);
    }

    void Update(float dt) override {
        PhysicsUIElement::Update(dt);
        scanlineOffset += dt * 120.0f;
        if (scanlineOffset > elementSize.y) scanlineOffset = 0.0f;
        
        if (GetRandomValue(0, 100) > 95) InternalUpdateHistory();
    }

    void Render(ImDrawList* drawList) override {
        PhysicsUIElement::Render(drawList);
        
        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};
        
        DrawTacticalGrid(drawList, pMin, pMax);

        float lineY = pMin.y + scanlineOffset * currentScale;
        if (lineY < pMax.y) {
            drawList->AddLine({pMin.x, lineY}, {pMax.x, lineY}, ImColor(0.0f, 1.0f, 1.0f, 0.2f * masterAlpha), 1.5f);
        }

        for (size_t i = 0; i < historyBuffer.size(); ++i) {
            float h = (historyBuffer[i] / 100.0f) * 40.0f;
            float bx = pMin.x + 15 + (i * 12);
            float by = pMax.y - 10;
            drawList->AddRectFilled({bx, by}, {bx + 8, by - h}, ImColor(0.0f, 1.0f, 1.0f, 0.5f * masterAlpha));
        }
    }
};
struct GlitchProfile {
    float intensity;
    float frequency;
    float jitterSpeed;
    float driftAmount;
    bool isHeavyGlitching;
    float heavyTimer;

    GlitchProfile() : intensity(0.0f), frequency(0.1f), jitterSpeed(20.0f), driftAmount(5.0f), 
                      isHeavyGlitching(false), heavyTimer(0.0f) {}
};

class UIEffectController {
private:
    float timeAccumulator;
    float noiseSeed;
    GlitchProfile glitch;

    float InternalGetSmoothNoise(float t) {
        return (sinf(t) + sinf(t * 1.5f) + sinf(t * 2.1f)) / 3.0f;
    }

    float InternalGetStepNoise(float t, float steps) {
        return floorf(InternalGetSmoothNoise(t) * steps) / steps;
    }

    ImU32 InternalApplyChromatic(ImU32 col, float shift, int channel) {
        ImVec4 c = ImGui::ColorConvertU32ToFloat4(col);
        if (channel == 0) c.x = fminf(1.0f, c.x + shift); // Red shift
        else if (channel == 2) c.z = fminf(1.0f, c.z + shift); // Blue shift
        return ImGui::ColorConvertFloat4ToU32(c);
    }

public:
    UIEffectController() : timeAccumulator(0.0f), noiseSeed(0.0f) {
        noiseSeed = (float)GetRandomValue(0, 1000) / 1000.0f;
    }

    void Update(float dt) {
        timeAccumulator += dt;
        
        if (glitch.isHeavyGlitching) {
            glitch.heavyTimer -= dt;
            if (glitch.heavyTimer <= 0.0f) glitch.isHeavyGlitching = false;
        } else if (GetRandomValue(0, 1000) > 995) {
            glitch.isHeavyGlitching = true;
            glitch.heavyTimer = (float)GetRandomValue(5, 20) / 100.0f;
        }

        float targetIntensity = glitch.isHeavyGlitching ? 1.0f : 0.02f;
        glitch.intensity = targetIntensity + InternalGetSmoothNoise(timeAccumulator * 10.0f) * 0.1f;
    }

    void DrawScanlines(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax, float alpha) {
        float spacing = 3.0f;
        ImU32 lineCol = ImColor(0.0f, 1.0f, 1.0f, 0.08f * alpha);
        for (float y = pMin.y; y < pMax.y; y += spacing) {
            float wave = sinf(y * 0.1f + timeAccumulator * 5.0f) * 0.5f + 0.5f;
            dl->AddLine({pMin.x, y}, {pMax.x, y}, ImColor(0.0f, 1.0f, 1.0f, 0.04f * wave * alpha));
        }
    }

    void DrawGlitchRect(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax, ImU32 color, float masterAlpha) {
        if (glitch.intensity < 0.05f) return;

        int blockCount = glitch.isHeavyGlitching ? 8 : 2;
        for (int i = 0; i < blockCount; i++) {
            float offset = InternalGetSmoothNoise(timeAccumulator * glitch.jitterSpeed + i) * glitch.driftAmount * glitch.intensity;
            float blockHeight = (float)GetRandomValue(2, 15);
            float yPos = InternalGetLinearInterp(pMin.y, pMax.y, (float)GetRandomValue(0, 100) / 100.0f);

            ImVec2 bMin = {pMin.x + offset, yPos};
            ImVec2 bMax = {pMax.x + offset, yPos + blockHeight};
            
            ImU32 gCol = InternalApplyChromatic(color, 0.4f * glitch.intensity, i % 3);
            dl->AddRectFilled(bMin, bMax, ImColor(ImGui::ColorConvertU32ToFloat4(gCol).x, 
                                                ImGui::ColorConvertU32ToFloat4(gCol).y, 
                                                ImGui::ColorConvertU32ToFloat4(gCol).z, 0.3f * masterAlpha));
        }
    }

    void DrawVignetteGlow(ImDrawList* dl, ImVec2 center, float radius, ImU32 color, float alpha) {
        int segments = 32;
        for (int i = 0; i < 5; i++) {
            float r = radius + (i * 15.0f);
            float a = (0.15f / (i + 1)) * alpha;
            dl->AddCircle(center, r, ImColor(ImGui::ColorConvertU32ToFloat4(color).x, 
                                           ImGui::ColorConvertU32ToFloat4(color).y, 
                                           ImGui::ColorConvertU32ToFloat4(color).z, a), segments, 2.0f);
        }
    }

    float GetGlitchIntensity() const { return glitch.intensity; }
};

class AdvancedEffectElement : public TacticalFrameElement {
protected:
    UIEffectController effects;
    float flickerTimer;
    float currentFlicker;

    void InternalUpdateFlicker(float dt) {
        flickerTimer += dt;
        if (GetRandomValue(0, 100) > 98) {
            currentFlicker = (float)GetRandomValue(30, 80) / 100.0f;
        } else {
            currentFlicker = InternalApplyExponentialDecay(currentFlicker, 1.0f, 20.0f, dt);
        }
    }

    void DrawHologramInterference(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax) {
        float noise = effects.GetGlitchIntensity();
        if (noise > 0.5f) {
            float lineY = pMin.y + (pMax.y - pMin.y) * ((float)GetRandomValue(0, 100) / 100.0f);
            dl->AddLine({pMin.x - 20, lineY}, {pMax.x + 20, lineY}, ImColor(1.0f, 1.0f, 1.0f, 0.4f * masterAlpha), 1.0f);
        }
    }

public:
    AdvancedEffectElement(std::string id, ImVec2 pos, ImVec2 size)
        : TacticalFrameElement(id, pos, size) {
        flickerTimer = 0.0f;
        currentFlicker = 1.0f;
    }

    void Update(float dt) override {
        TacticalFrameElement::Update(dt);
        effects.Update(dt);
        InternalUpdateFlicker(dt);
    }

    void Render(ImDrawList* drawList) override {
        float originalAlpha = masterAlpha;
        masterAlpha *= currentFlicker;

        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};

        effects.DrawGlitchRect(drawList, pMin, pMax, ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z), masterAlpha);
        
        TacticalFrameElement::Render(drawList);
        
        effects.DrawScanlines(drawList, pMin, pMax, masterAlpha);
        DrawHologramInterference(drawList, pMin, pMax);

        if (interaction.isHovered) {
            ImVec2 center = {pMin.x + (pMax.x - pMin.x) * 0.5f, pMin.y + (pMax.y - pMin.y) * 0.5f};
            effects.DrawVignetteGlow(drawList, center, (pMax.x - pMin.x) * 0.6f, 
                                     ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z), 
                                     interaction.hoverTransition * masterAlpha);
        }

        masterAlpha = originalAlpha;
    }
};
struct ParallaxLayerConfig {
    float movementIntensity;
    float lerpSpeed;
    float floatingAmplitude;
    float floatingFrequency;
    float depthScale;
    float depthAlpha;

    ParallaxLayerConfig(float intensity, float lSpeed, float amp, float freq, float dScale, float dAlpha)
        : movementIntensity(intensity), lerpSpeed(lSpeed), floatingAmplitude(amp), 
          floatingFrequency(freq), depthScale(dScale), depthAlpha(dAlpha) {}
};

class ParallaxController {
private:
    std::map<UILayerDepth, ParallaxLayerConfig> layerConfigs;
    Vector2 smoothedMouseOffset;
    float globalTime;

    void InternalInitConfigs() {
        layerConfigs.emplace(UILayerDepth::BACKGROUND_STATIC, ParallaxLayerConfig(0.015f, 2.5f, 3.5f, 0.4f, 0.95f, 0.6f));
        layerConfigs.emplace(UILayerDepth::MIDGROUND_DYNAMIC, ParallaxLayerConfig(0.045f, 4.2f, 7.0f, 0.7f, 1.0f, 0.85f));
        layerConfigs.emplace(UILayerDepth::FOREGROUND_HUD, ParallaxLayerConfig(0.085f, 6.5f, 12.0f, 1.1f, 1.08f, 1.0f));
        layerConfigs.emplace(UILayerDepth::OVERLAY_GLITCH, ParallaxLayerConfig(0.125f, 9.0f, 18.0f, 1.5f, 1.15f, 1.0f));
    }

    Vector2 InternalCalculateFloating(const ParallaxLayerConfig& cfg, float time, uint32_t seed) {
        float phase = (float)(seed % 100);
        float x = sinf(time * cfg.floatingFrequency + phase) * cfg.floatingAmplitude;
        float y = cosf(time * cfg.floatingFrequency * 0.8f + phase) * cfg.floatingAmplitude;
        return {x, y};
    }

public:
    ParallaxController() : globalTime(0.0f) {
        smoothedMouseOffset = {0, 0};
        InternalInitConfigs();
    }

    void Update(float dt) {
        globalTime += dt;
        Vector2 mousePos = GetMousePosition();
        Vector2 targetOffset = {
            mousePos.x - SCREEN_WIDTH * 0.5f,
            mousePos.y - SCREEN_HEIGHT * 0.5f
        };

        smoothedMouseOffset.x = InternalGetLinearInterp(smoothedMouseOffset.x, targetOffset.x, 5.0f * dt);
        smoothedMouseOffset.y = InternalGetLinearInterp(smoothedMouseOffset.y, targetOffset.y, 5.0f * dt);
    }

    void ApplyLayerTransform(UIElement* element, ImVec2& outPos, float& outScale, float& outAlpha) {
        auto it = layerConfigs.find(element->GetLayer());
        if (it == layerConfigs.end()) return;

        const auto& cfg = it->second;
        uint32_t seed = (uint32_t)element->GetID().length();
        
        Vector2 floating = InternalCalculateFloating(cfg, globalTime, seed);
        
        outPos.x += (smoothedMouseOffset.x * cfg.movementIntensity) + floating.x;
        outPos.y += (smoothedMouseOffset.y * cfg.movementIntensity) + floating.y;
        
        outScale *= cfg.depthScale;
        outAlpha *= cfg.depthAlpha;
    }
};

class LayeredPhysicsElement : public AdvancedEffectElement {
protected:
    static ParallaxController globalParallax;
    bool enableParallax;

public:
    LayeredPhysicsElement(std::string id, ImVec2 pos, ImVec2 size)
        : AdvancedEffectElement(id, pos, size), enableParallax(true) {}

    void Update(float dt) override {
        AdvancedEffectElement::Update(dt);
        globalParallax.Update(dt);
    }

    void LateUpdate(float dt) override {
        AdvancedEffectElement::LateUpdate(dt);
        
        if (enableParallax) {
            ImVec2 parallaxPos = currentPosition;
            float parallaxScale = currentScale;
            float parallaxAlpha = masterAlpha;

            globalParallax.ApplyLayerTransform(this, parallaxPos, parallaxScale, parallaxAlpha);
            
            currentPosition = parallaxPos;
            currentScale = parallaxScale;
            masterAlpha = parallaxAlpha;
        }
    }
};

ParallaxController LayeredPhysicsElement::globalParallax;

class HyperLinkElement : public LayeredPhysicsElement {
private:
    float pulseAnim;
    bool isLinked;

    void DrawLinkCores(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax) {
        float size = 10.0f + sinf(internalTimer * 5.0f) * 2.0f;
        ImU32 col = ImColor(style.primaryColor.x, style.primaryColor.y, style.primaryColor.z, (0.5f + pulseAnim * 0.5f) * masterAlpha);
        
        dl->AddCircleFilled({pMin.x, pMin.y + (pMax.y - pMin.y) * 0.5f}, size, col, 4);
        dl->AddCircle({pMin.x, pMin.y + (pMax.y - pMin.y) * 0.5f}, size + 5.0f, col, 4, 1.5f);
    }

    void DrawDataStream(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax) {
        if (!interaction.isHovered) return;
        
        float streamY = pMin.y + 30.0f;
        for (int i = 0; i < 5; i++) {
            float xOff = sinf(internalTimer * 10.0f + i) * 5.0f;
            dl->AddText(NULL, 10.0f, {pMin.x + 15 + xOff, streamY + (i * 12)}, 
                        ImColor(0.0f, 1.0f, 1.0f, 0.4f * masterAlpha), "0101101010110");
        }
    }

public:
    HyperLinkElement(std::string id, ImVec2 pos, ImVec2 size)
        : LayeredPhysicsElement(id, pos, size), pulseAnim(0.0f), isLinked(false) {
        style.primaryColor = ImVec4(0.8f, 1.0f, 0.0f, 1.0f); // Amber color for links
    }

    void Update(float dt) override {
        LayeredPhysicsElement::Update(dt);
        pulseAnim = InternalGetLinearInterp(pulseAnim, interaction.isHovered ? 1.0f : 0.2f, 6.0f * dt);
    }

    void Render(ImDrawList* drawList) override {
        LayeredPhysicsElement::Render(drawList);
        
        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};
        
        DrawLinkCores(drawList, pMin, pMax);
        DrawDataStream(drawList, pMin, pMax);

        if (interaction.isHovered) {
            drawList->AddRect(pMin, pMax, ImColor(1.0f, 0.8f, 0.0f, 0.6f * masterAlpha), style.rounding, 0, 1.5f);
        }
    }
};

// *************************
struct PerformanceConfig {
    bool enableFrameSkip;
    int updateFrequency;
    float visibilityThreshold;
    bool useLowResEffects;
    uint32_t activeElementCount;

    PerformanceConfig() : enableFrameSkip(true), updateFrequency(2), visibilityThreshold(0.01f), 
                         useLowResEffects(false), activeElementCount(0) {}
};

class OptimizationController {
private:
    PerformanceConfig config;
    uint32_t frameCounter;

public:
    OptimizationController() : frameCounter(0) {}

    bool ShouldUpdate(uint32_t elementHash) {
        if (!config.enableFrameSkip) return true;
        return (frameCounter + elementHash) % config.updateFrequency == 0;
    }

    bool IsCullable(float alpha, ImVec2 pos, ImVec2 size) {
        if (alpha < config.visibilityThreshold) return true;
        if (pos.x + size.x < 0 || pos.x > SCREEN_WIDTH || 
            pos.y + size.y < 0 || pos.y > SCREEN_HEIGHT) return true;
        return false;
    }

    void IncrementFrame() { frameCounter++; }
    void UpdateConfig(const PerformanceConfig& newCfg) { config = newCfg; }
};

class ProceduralEntropy {
private:
    float seed;
    float instability;

public:
    ProceduralEntropy(float s) : seed(s), instability(0.1f) {}

    float GetNoise(float time, float speed, float range) {
        return sinf(time * speed + seed) * cosf(time * speed * 0.7f + seed) * range;
    }

    ImVec4 ApplyColorJitter(ImVec4 base, float time) {
        float j = GetNoise(time, 10.0f, instability);
        return ImVec4(base.x + j, base.y + j, base.z + j, base.w);
    }
};

class FinalIntegratedElement : public HyperLinkElement {
private:
    OptimizationController opt;
    ProceduralEntropy entropy;
    float internalBootSequence;

    void DrawSubtleFrameGlow(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax) {
        float t = (float)GetTime();
        float g = (sinf(t * 2.0f) * 0.5f + 0.5f) * 0.1f;
        dl->AddRect(pMin, pMax, ImColor(1.0f, 1.0f, 1.0f, g * masterAlpha), style.rounding, 0, 3.0f);
    }

    void DrawDiagnosticText(ImDrawList* dl, ImVec2 pMin, ImVec2 pMax) {
        if (internalBootSequence < 1.0f) {
            char buf[32];
            sprintf(buf, "LOADING: %.1f%%", internalBootSequence * 100.0f);
            dl->AddText(NULL, 10.0f, {pMin.x + 5, pMax.y + 5}, ImColor(0.0f, 1.0f, 0.5f, masterAlpha), buf);
        }
    }

public:
    FinalIntegratedElement(std::string id, ImVec2 pos, ImVec2 size)
        : HyperLinkElement(id, pos, size), entropy((float)id.length()), internalBootSequence(0.0f) {
        style.rounding = 4.0f;
    }

    void Update(float dt) override {
        if (internalBootSequence < 1.0f) {
            internalBootSequence += dt * 0.5f;
            if (internalBootSequence > 1.0f) internalBootSequence = 1.0f;
            masterAlpha = internalBootSequence;
        }

        if (opt.ShouldUpdate((uint32_t)elementID.length())) {
            HyperLinkElement::Update(dt);
        }
        
        float jitterX = entropy.GetNoise((float)GetTime(), 5.0f, 0.5f);
        basePosition.x += jitterX * dt;
    }

    void Render(ImDrawList* drawList) override {
        ImVec2 pMin = currentPosition;
        ImVec2 pMax = {pMin.x + elementSize.x * currentScale, pMin.y + elementSize.y * currentScale};

        if (opt.IsCullable(masterAlpha, pMin, {pMax.x - pMin.x, pMax.y - pMin.y})) return;

        DrawSubtleFrameGlow(drawList, pMin, pMax);
        HyperLinkElement::Render(drawList);
        DrawDiagnosticText(drawList, pMin, pMax);
    }
};

void MainLoopExecution() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AAA_DIEGETIC_UI_FINAL");
    SetTargetFPS(144);
    rlImGuiSetup(true);

    g_SystemHUD.InitializeSystem();

    auto coreOS = std::make_unique<FinalIntegratedElement>("HYPERION_CORE_v8", ImVec2(50, 50), ImVec2(320, 180));
    coreOS->SetLayer(UILayerDepth::FOREGROUND_HUD);
    
    auto wepLink = std::make_unique<FinalIntegratedElement>("WEAPON_LINK_ESTABLISHED", ImVec2(910, 50), ImVec2(320, 200));
    wepLink->SetLayer(UILayerDepth::FOREGROUND_HUD);

    auto navGrid = std::make_unique<FinalIntegratedElement>("GLOBAL_NAV_GRID", ImVec2(50, 480), ImVec2(350, 200));
    navGrid->SetLayer(UILayerDepth::MIDGROUND_DYNAMIC);

    auto bioStream = std::make_unique<FinalIntegratedElement>("BIO_METRIC_SENSORS", ImVec2(880, 500), ImVec2(350, 180));
    bioStream->SetLayer(UILayerDepth::MIDGROUND_DYNAMIC);

    g_SystemHUD.RegisterElement(std::move(coreOS));
    g_SystemHUD.RegisterElement(std::move(wepLink));
    g_SystemHUD.RegisterElement(std::move(navGrid));
    g_SystemHUD.RegisterElement(std::move(bioStream));

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        g_SystemHUD.Update(dt);

        BeginDrawing();
        ClearBackground({5, 10, 15, 255}); // Deep dark space color

        rlImGuiBegin();
        g_SystemHUD.Render();
        rlImGuiEnd();

        DrawFPS(10, SCREEN_HEIGHT - 25);
        EndDrawing();
    }

    g_SystemHUD.ShutdownSystem();
    rlImGuiShutdown();
    CloseWindow();
}

// int main() {
    // MainLoopExecution();
   // return 0;
// }

#include <memory>

UIManager g_SystemHUD; 

void InitDecoration() {
    g_SystemHUD.InitializeSystem();
    
    auto coreOS = std::make_unique<FinalIntegratedElement>("HYPERION_v2.0", ImVec2(40, 40), ImVec2(300, 160));
    coreOS->SetLayer(UILayerDepth::FOREGROUND_HUD);
    g_SystemHUD.RegisterElement(std::move(coreOS));
}

void UpdateDecoration(float dt) {
    g_SystemHUD.Update(dt);
}

void DrawDecoration() {
    g_SystemHUD.Render();
}

// 나 이거 짜는데 존나 힘들었어
// 물론 AI가 좀 도와주긴했는데
// AI써도 1000줄 코드 짜는건 거의 불가능한듯 ㄷㄷ
// 70프로는 내가하고 30프로는 AI가함
// 이코드 ㄹㅇ 어케돌아가는거임?
// 볼때마다 신기해
// 48시간 걸린 코드