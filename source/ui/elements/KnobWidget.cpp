#include "KnobWidget.h"
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void KnobWidget::draw(const char* label,
                       float normalizedValue,
                       const Marking* markings,
                       int markingCount,
                       const char* displayValue,
                       const char* keyHint) const {
    // Clamp value
    float val = normalizedValue;
    if (val < 0.0f) val = 0.0f;
    if (val > 1.0f) val = 1.0f;

    ImGui::BeginChild(label, ImVec2(kBoxWidth, kBoxHeight), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    float contentWidth = ImGui::GetContentRegionAvail().x;

    // Layout: label on top, knob in center, value on bottom
    float labelH = ImGui::GetTextLineHeight() + 4.0f;
    float valueH = ImGui::GetTextLineHeight() + 2.0f;
    float knobAreaH = kBoxHeight - labelH - valueH;
    float knobCenterX = cursor.x + contentWidth * 0.5f;
    float knobCenterY = cursor.y + labelH + knobAreaH * 0.5f;
    ImVec2 knobCenter(knobCenterX, knobCenterY);

    // ── Label (top row) ──
    ImGui::Text("%s", label);
    if (keyHint && keyHint[0]) {
        float hintW = ImGui::CalcTextSize(keyHint).x;
        ImGui::SameLine(contentWidth - hintW - ImGui::GetStyle().WindowPadding.x);
        ImGui::TextColored(ImVec4(0.35f, 0.35f, 0.35f, 1.0f), "%s", keyHint);
    }

    // Reserve space for the knob area
    ImGui::Dummy(ImVec2(0.0f, knobAreaH));

    // ── Value display (bottom row) ──
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "%s", displayValue);

    // ── Draw knob visuals ──

    // Knob background circle
    dl->AddCircleFilled(knobCenter, kKnobRadius, colKnobBg());
    dl->AddCircle(knobCenter, kKnobRadius, colKnobBorder(), 0, 2.0f);

    // Arc sweep: 270° clockwise from 135° to 405° (= 45° mod 360°)
    // Gap is at the top (between 45° and 135°)
    float arcStartDeg = 135.0f;
    float arcSweepDeg = 270.0f;

    // Draw full arc background track
    drawArc(dl, knobCenter, kArcOuterRadius,
            arcStartDeg, arcStartDeg + arcSweepDeg,
            colArcBg(), kArcThickness);

    // Draw marker line from center outward
    float markerAngleDeg = arcStartDeg + val * arcSweepDeg;
    ImVec2 markerInner = pointOnCircle(knobCenter, 6.0f, markerAngleDeg);
    ImVec2 markerOuter = pointOnCircle(knobCenter, kMarkerLength, markerAngleDeg);
    dl->AddLine(markerInner, markerOuter, colMarker(), kMarkerThickness);

    // Draw tick marks and labels
    for (int i = 0; i < markingCount; ++i) {
        float tickAngleDeg = arcStartDeg + markings[i].normalizedPos * arcSweepDeg;
        ImVec2 tickInner = pointOnCircle(knobCenter, kArcOuterRadius + 2.0f, tickAngleDeg);
        ImVec2 tickOuter = pointOnCircle(knobCenter, kArcOuterRadius + 2.0f + kTickLength, tickAngleDeg);
        dl->AddLine(tickInner, tickOuter, colTick(), kTickThickness);

        // Label position: outside the tick mark
        ImVec2 labelPos = pointOnCircle(knobCenter, kArcOuterRadius + 2.0f + kTickLength + kLabelOffset, tickAngleDeg);
        const char* text = markings[i].label;
        float textW = ImGui::CalcTextSize(text).x;
        float textH = ImGui::GetTextLineHeight();
        // Center text on the tick position
        dl->AddText(ImVec2(labelPos.x - textW * 0.5f, labelPos.y - textH * 0.5f),
                    colTextDim(), text);
    }

    ImGui::EndChild();
}

float KnobWidget::normalizedToAngle(float normalized) const {
    float arcStartDeg = 135.0f;
    float arcSweepDeg = 270.0f;
    return arcStartDeg + normalized * arcSweepDeg;
}

ImVec2 KnobWidget::pointOnCircle(ImVec2 center, float radius, float angleDeg) const {
    float angleRad = angleDeg * (float)M_PI / 180.0f;
    float x = center.x + radius * cosf(angleRad);
    float y = center.y + radius * sinf(angleRad);
    return ImVec2(x, y);
}

void KnobWidget::drawArc(ImDrawList* dl, ImVec2 center, float radius,
                          float startAngle, float endAngle, ImU32 color, float thickness) const {
    // Approximate arc with line segments
    float sweep = endAngle - startAngle;
    if (sweep <= 0.0f) return;

    int segments = (int)(sweep / 5.0f); // ~5° per segment
    if (segments < 8) segments = 8;

    float step = sweep / (float)segments;
    ImVec2 prev = pointOnCircle(center, radius, startAngle);
    for (int i = 1; i <= segments; ++i) {
        float angle = startAngle + (float)i * step;
        ImVec2 curr = pointOnCircle(center, radius, angle);
        dl->AddLine(prev, curr, color, thickness);
        prev = curr;
    }
}
