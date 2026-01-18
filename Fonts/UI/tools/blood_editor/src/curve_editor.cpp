#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace BloodEditor {

class CurveEditor {
public:
    enum class CurveType {
        LINEAR,
        BEZIER,
        CATMULL_ROM,
        BSPLINE,
        HERMITE
    };
    
    enum class EditMode {
        CREATE,
        EDIT_POINTS,
        EDIT_TANGENTS,
        DELETE_POINTS
    };
    
    struct CurveSettings {
        CurveType type = CurveType::BEZIER;
        float thickness = 2.0f;
        glm::vec4 color = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
        bool smooth = true;
        float tension = 0.5f;
        float bias = 0.0f;
        bool closed = false;
        float subdivisionLevel = 10.0f;
        bool showTangents = false;
        bool showControlPoints = true;
        bool showCurve = true;
        float pointSize = 6.0f;
        float tangentLength = 50.0f;
    };
    
    struct ControlPoint {
        glm::vec2 position;
        glm::vec2 tangentIn;
        glm::vec2 tangentOut;
        bool selected;
        bool locked;
        float weight;
    };
    
    CurveEditor();
    ~CurveEditor() = default;
    
    void update(float deltaTime);
    void render();
    bool handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed);
    
    void setTargetEffect(BloodEffect* effect) { targetEffect_ = effect; }
    
    void clearCurrentCurve();
    void finishCurrentCurve();
    void deleteSelectedPoints();
    
    bool isEditing() const { return editing_; }
    EditMode getEditMode() const { return editMode_; }
    void setEditMode(EditMode mode) { editMode_ = mode; }
    
    const CurveSettings& getSettings() const { return settings_; }
    void setSettings(const CurveSettings& settings) { settings_ = settings; }
    
    // Curve operations
    void smoothCurve();
    void simplifyCurve(float tolerance = 1.0f);
    void subdivideCurve();
    void reverseCurve();
    void mirrorCurve(bool horizontal, bool vertical);
    
    // Import/Export
    void loadCurveFromFile(const std::string& filename);
    void saveCurveToFile(const std::string& filename);
    
    // Presets
    void loadPreset(const std::string& presetName);
    void savePreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
private:
    BloodEffect* targetEffect_;
    BloodCurve currentCurve_;
    std::vector<ControlPoint> controlPoints_;
    CurveSettings settings_;
    EditMode editMode_;
    bool editing_;
    int selectedPoint_;
    glm::vec2 mouseOffset_;
    glm::vec2 lastMousePos_;
    
    // Animation
    float animationTime_;
    bool animating_;
    float animationSpeed_;
    
    // History for undo/redo
    std::vector<std::vector<ControlPoint>> history_;
    int historyIndex_;
    static const int MAX_HISTORY_SIZE = 50;
    
    // Helper methods
    int findNearestPoint(const glm::vec2& position, float threshold = 10.0f);
    int findNearestTangent(const glm::vec2& position, float threshold = 10.0f);
    
    void renderCurve(const BloodCurve& curve);
    void renderControlPoints();
    void renderTangents();
    void renderCurvePreview();
    
    // Curve generation
    std::vector<glm::vec2> generateLinearCurve(const std::vector<ControlPoint>& points);
    std::vector<glm::vec2> generateBezierCurve(const std::vector<ControlPoint>& points);
    std::vector<glm::vec2> generateCatmullRomCurve(const std::vector<ControlPoint>& points);
    std::vector<glm::vec2> generateBSplineCurve(const std::vector<ControlPoint>& points);
    std::vector<glm::vec2> generateHermiteCurve(const std::vector<ControlPoint>& points);
    
    // Math utilities
    glm::vec2 evaluateBezier(const std::vector<glm::vec2>& points, float t);
    glm::vec2 evaluateCatmullRom(const glm::vec2& p0, const glm::vec2& p1, 
                                 const glm::vec2& p2, const glm::vec2& p3, float t);
    glm::vec2 evaluateHermite(const glm::vec2& p0, const glm::vec2& p1,
                             const glm::vec2& t0, const glm::vec2& t1, float t);
    
    // History management
    void saveToHistory();
    void undo();
    void redo();
    bool canUndo() const { return historyIndex_ > 0; }
    bool canRedo() const { return historyIndex_ < history_.size() - 1; }
    
    // Preset management
    void initializeDefaultPresets();
    std::map<std::string, BloodCurve> presets_;
    
    // UI helpers
    void renderCurveTypeUI();
    void renderEditModeUI();
    void renderCurvePropertiesUI();
    void renderHistoryUI();
    void renderPresetUI();
};

CurveEditor::CurveEditor() 
    : targetEffect_(nullptr), editing_(false), selectedPoint_(-1), editMode_(EditMode::CREATE),
      animationTime_(0.0f), animating_(false), animationSpeed_(1.0f), historyIndex_(-1) {
    
    currentCurve_.thickness = settings_.thickness;
    currentCurve_.color = settings_.color;
    currentCurve_.name = "Blood Curve";
    
    initializeDefaultPresets();
}

void CurveEditor::update(float deltaTime) {
    // Update curve properties if needed
    if (editing_ && targetEffect_) {
        // Could animate or modify curve properties here
    }
}

void CurveEditor::render() {
    if (!editing_) return;
    
    // Render current curve being edited
    renderCurve(currentCurve_);
    
    // Render control points
    renderControlPoints();
}

bool CurveEditor::handleInput(const glm::vec2& mousePos, bool mouseDown, bool mousePressed) {
    if (!editing_) return false;
    
    if (mousePressed) {
        // Start new curve or select point
        if (currentCurve_.points.empty()) {
            // Start new curve
            currentCurve_.points.push_back(mousePos);
            return true;
        } else {
            // Check if clicking near existing point
            int nearestPoint = findNearestPoint(mousePos);
            if (nearestPoint != -1) {
                selectedPoint_ = nearestPoint;
                mouseOffset_ = currentCurve_.points[nearestPoint] - mousePos;
            } else {
                // Add new point to curve
                currentCurve_.points.push_back(mousePos);
                return true;
            }
        }
    }
    
    if (mouseDown && selectedPoint_ != -1) {
        // Move selected point
        currentCurve_.points[selectedPoint_] = mousePos + mouseOffset_;
        return true;
    }
    
    if (!mouseDown) {
        selectedPoint_ = -1;
    }
    
    return false;
}

void CurveEditor::clearCurrentCurve() {
    currentCurve_.points.clear();
    selectedPoint_ = -1;
}

void CurveEditor::finishCurrentCurve() {
    if (!currentCurve_.points.empty() && targetEffect_) {
        targetEffect_->addCurve(currentCurve_);
        clearCurrentCurve();
    }
}

int CurveEditor::findNearestPoint(const glm::vec2& position, float threshold) {
    for (size_t i = 0; i < currentCurve_.points.size(); ++i) {
        float distance = glm::length(currentCurve_.points[i] - position);
        if (distance < threshold) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void CurveEditor::renderCurve(const BloodCurve& curve) {
    if (curve.points.size() < 2) return;
    
    // Render curve as connected line segments
    glBegin(GL_LINE_STRIP);
    glColor4f(curve.color.r, curve.color.g, curve.color.b, curve.color.a);
    glLineWidth(curve.thickness);
    
    for (const auto& point : curve.points) {
        glVertex2f(point.x, point.y);
    }
    
    glEnd();
    glLineWidth(1.0f);
}

void CurveEditor::renderControlPoints() {
    // Render control points as small squares
    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    
    for (size_t i = 0; i < currentCurve_.points.size(); ++i) {
        const auto& point = currentCurve_.points[i];
        float size = (i == selectedPoint_) ? 8.0f : 5.0f;
        
        glVertex2f(point.x - size, point.y - size);
        glVertex2f(point.x + size, point.y - size);
        glVertex2f(point.x + size, point.y + size);
        glVertex2f(point.x - size, point.y + size);
    }
    
    glEnd();
}

} // namespace BloodEditor