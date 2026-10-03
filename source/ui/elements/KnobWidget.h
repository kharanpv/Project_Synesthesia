#pragma once

#include <imgui.h>

// ────────────────────────────────────────────────────────────────────────────
// KnobWidget
// A visual rotary knob for continuous parameters. Draws a circular knob
// with an arc indicator, tick marks, and a line marker showing current value.
// Uses the 80s/90s terminal HUD aesthetic (black bg, white/cyan accents).
//
// The arc sweeps 270° with a 90° gap at the top for the label.
// Input is normalized (0.0 to 1.0); the caller handles conversion to
// the actual parameter range.
// ────────────────────────────────────────────────────────────────────────────
class KnobWidget {
public:
    struct Marking {
        const char* label;       // Text shown at this tick (e.g., "20Hz")
        float normalizedPos;     // 0.0 = arc start, 1.0 = arc end
    };

    // Draw the knob widget.
    // - label: parameter name (rendered above the knob)
    // - normalizedValue: 0.0 to 1.0 current position
    // - markings: array of tick mark labels around the arc
    // - markingCount: number of markings
    // - displayValue: formatted numerical readout (e.g., "1430.89 Hz")
    // - keyHint: key binding text (e.g., "[A]/[Z]")
    void draw(const char* label,
              float normalizedValue,
              const Marking* markings,
              int markingCount,
              const char* displayValue,
              const char* keyHint) const;

private:
    // Internal constants (self-contained, not in TerminalStyle)
    static constexpr float kKnobDiameter       = 100.0f;
    static constexpr float kKnobRadius         = kKnobDiameter * 0.5f;
    static constexpr float kArcThickness       = 4.0f;
    static constexpr float kArcInnerRadius     = kKnobRadius - 8.0f;
    static constexpr float kArcOuterRadius     = kKnobRadius - 2.0f;
    static constexpr float kMarkerLength       = kArcInnerRadius - 4.0f;
    static constexpr float kMarkerThickness    = 2.0f;
    static constexpr float kTickLength         = 6.0f;
    static constexpr float kTickThickness      = 1.0f;
    static constexpr float kLabelOffset        = 16.0f;  // distance from arc edge to label text
    static constexpr float kBoxWidth           = 340.0f;
    static constexpr float kBoxHeight          = 300.0f;

    // Arc angle range in radians (screen coordinates: 0 = right, clockwise)
    // Gap at top: arc goes from 225° (bottom-left) to 315° (bottom-right)
    // In radians: 225° = 5π/4, 315° = 7π/4
    static constexpr float kArcStartDeg        = 225.0f;
    static constexpr float kArcEndDeg          = 315.0f;
    static constexpr float kArcSweepDeg        = kArcEndDeg - kArcStartDeg + 360.0f; // 270°
    // Actual: we go from 225° clockwise 270° to 315° (passing through 0°)

    // Colors (ImU32 for ImDrawList) — all unified to #505050
    static ImU32 colKnobBg()      { return IM_COL32(0, 0, 0, 255); }
    static ImU32 colKnobBorder()  { return IM_COL32(80, 80, 80, 255); }
    static ImU32 colArcBg()       { return IM_COL32(80, 80, 80, 255); }
    static ImU32 colMarker()      { return IM_COL32(80, 80, 80, 255); }
    static ImU32 colTick()        { return IM_COL32(80, 80, 80, 255); }
    static ImU32 colText()        { return IM_COL32(230, 230, 230, 255); }
    static ImU32 colTextDim()     { return IM_COL32(80, 80, 80, 255); }

    // Convert normalized value (0-1) to angle in degrees (screen coords)
    float normalizedToAngle(float normalized) const;

    // Get a point on the circle at a given angle (degrees) and radius
    ImVec2 pointOnCircle(ImVec2 center, float radius, float angleDeg) const;

    // Draw an arc from startAngle to endAngle (degrees) at the given radius
    void drawArc(ImDrawList* dl, ImVec2 center, float radius,
                 float startAngle, float endAngle, ImU32 color, float thickness) const;
};
