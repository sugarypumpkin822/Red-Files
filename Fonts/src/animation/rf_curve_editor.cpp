#include "rf_curve_editor.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

CurveEditor::CurveEditor()
    : m_selectedCurve(nullptr)
    , m_selectedPointIndex(-1)
    , m_editMode(EditMode::None)
    , m_snapToGrid(true)
    , m_gridSize(10.0f)
    , m_zoomLevel(1.0f)
    , m_panOffset(0.0f, 0.0f)
    , m_showGrid(true)
    , m_showTangents(true)
    , m_tangentLength(50.0f)
{
    RF_LOG_INFO("CurveEditor created");
}

CurveEditor::~CurveEditor() {
    clear();
    RF_LOG_INFO("CurveEditor destroyed");
}

void CurveEditor::update() {
    handleInput();
    updateSelection();
    updateCurves();
}

void CurveEditor::render() {
    renderGrid();
    renderCurves();
    renderPoints();
    renderTangents();
    renderUI();
}

AnimationCurve* CurveEditor::createCurve(const std::string& name, CurveType type) {
    auto curve = RF_NEW(AnimationCurve);
    curve->setName(name);
    curve->setType(type);
    
    m_curves.push_back(curve);
    m_curveMap[name] = curve;
    
    RF_LOG_INFO("Created curve: %s", name.c_str());
    return curve;
}

void CurveEditor::destroyCurve(AnimationCurve* curve) {
    if (!curve) {
        return;
    }
    
    // Remove from curves list
    auto it = std::find(m_curves.begin(), m_curves.end(), curve);
    if (it != m_curves.end()) {
        m_curves.erase(it);
    }
    
    // Remove from map
    auto mapIt = m_curveMap.find(curve->getName());
    if (mapIt != m_curveMap.end()) {
        m_curveMap.erase(mapIt);
    }
    
    // Clear selection if needed
    if (m_selectedCurve == curve) {
        m_selectedCurve = nullptr;
        m_selectedPointIndex = -1;
    }
    
    RF_DELETE(curve);
    RF_LOG_INFO("Destroyed curve: %s", curve->getName().c_str());
}

void CurveEditor::destroyCurve(const std::string& name) {
    auto it = m_curveMap.find(name);
    if (it != m_curveMap.end()) {
        destroyCurve(it->second);
    }
}

AnimationCurve* CurveEditor::getCurve(const std::string& name) {
    auto it = m_curveMap.find(name);
    return (it != m_curveMap.end()) ? it->second : nullptr;
}

AnimationCurve* CurveEditor::getSelectedCurve() const {
    return m_selectedCurve;
}

void CurveEditor::selectCurve(AnimationCurve* curve) {
    m_selectedCurve = curve;
    m_selectedPointIndex = -1;
    
    if (curve) {
        RF_LOG_INFO("Selected curve: %s", curve->getName().c_str());
    }
}

void CurveEditor::selectPoint(int pointIndex) {
    if (m_selectedCurve && pointIndex >= 0 && 
        pointIndex < static_cast<int>(m_selectedCurve->getPointCount())) {
        m_selectedPointIndex = pointIndex;
        RF_LOG_INFO("Selected point %d on curve %s", 
                    pointIndex, m_selectedCurve->getName().c_str());
    }
}

void CurveEditor::addPoint(const Vec2& point) {
    if (!m_selectedCurve) {
        return;
    }
    
    Vec2 snappedPoint = m_snapToGrid ? snapToGrid(point) : point;
    m_selectedCurve->addPoint(snappedPoint);
    
    RF_LOG_INFO("Added point (%.2f, %.2f) to curve %s", 
                snappedPoint.x, snappedPoint.y, m_selectedCurve->getName().c_str());
}

void CurveEditor::removePoint(int pointIndex) {
    if (!m_selectedCurve || pointIndex < 0 || 
        pointIndex >= static_cast<int>(m_selectedCurve->getPointCount())) {
        return;
    }
    
    m_selectedCurve->removePoint(pointIndex);
    
    // Clear selection if removing selected point
    if (m_selectedPointIndex == pointIndex) {
        m_selectedPointIndex = -1;
    } else if (m_selectedPointIndex > pointIndex) {
        m_selectedPointIndex--;
    }
    
    RF_LOG_INFO("Removed point %d from curve %s", 
                pointIndex, m_selectedCurve->getName().c_str());
}

void CurveEditor::movePoint(int pointIndex, const Vec2& newPosition) {
    if (!m_selectedCurve || pointIndex < 0 || 
        pointIndex >= static_cast<int>(m_selectedCurve->getPointCount())) {
        return;
    }
    
    Vec2 snappedPosition = m_snapToGrid ? snapToGrid(newPosition) : newPosition;
    m_selectedCurve->setPoint(pointIndex, snappedPosition);
    
    RF_LOG_INFO("Moved point %d on curve %s to (%.2f, %.2f)", 
                pointIndex, m_selectedCurve->getName().c_str(), 
                snappedPosition.x, snappedPosition.y);
}

void CurveEditor::setPointTangent(int pointIndex, const Vec2& tangent) {
    if (!m_selectedCurve || pointIndex < 0 || 
        pointIndex >= static_cast<int>(m_selectedCurve->getPointCount())) {
        return;
    }
    
    m_selectedCurve->setTangent(pointIndex, tangent);
    
    RF_LOG_INFO("Set tangent for point %d on curve %s", 
                pointIndex, m_selectedCurve->getName().c_str());
}

void CurveEditor::setEditMode(EditMode mode) {
    m_editMode = mode;
    RF_LOG_INFO("Edit mode set to %d", static_cast<int>(mode));
}

void CurveEditor::setSnapToGrid(bool snap) {
    m_snapToGrid = snap;
    RF_LOG_INFO("Snap to grid set to %s", snap ? "true" : "false");
}

void CurveEditor::setGridSize(float size) {
    m_gridSize = size;
    RF_LOG_INFO("Grid size set to %.2f", size);
}

void CurveEditor::setZoomLevel(float zoom) {
    m_zoomLevel = Math::clamp(zoom, 0.1f, 10.0f);
    RF_LOG_INFO("Zoom level set to %.2f", m_zoomLevel);
}

void CurveEditor::setPanOffset(const Vec2& offset) {
    m_panOffset = offset;
}

void CurveEditor::setShowGrid(bool show) {
    m_showGrid = show;
}

void CurveEditor::setShowTangents(bool show) {
    m_showTangents = show;
}

void CurveEditor::clear() {
    for (auto& curve : m_curves) {
        if (curve) {
            RF_DELETE(curve);
        }
    }
    
    m_curves.clear();
    m_curveMap.clear();
    m_selectedCurve = nullptr;
    m_selectedPointIndex = -1;
    
    RF_LOG_INFO("Cleared all curves");
}

std::vector<AnimationCurve*> CurveEditor::getCurves() const {
    return m_curves;
}

std::vector<std::string> CurveEditor::getCurveNames() const {
    std::vector<std::string> names;
    names.reserve(m_curveMap.size());
    
    for (const auto& pair : m_curveMap) {
        names.push_back(pair.first);
    }
    
    return names;
}

Vec2 CurveEditor::screenToWorld(const Vec2& screenPos) const {
    return (screenPos - m_panOffset) / m_zoomLevel;
}

Vec2 CurveEditor::worldToScreen(const Vec2& worldPos) const {
    return worldPos * m_zoomLevel + m_panOffset;
}

Vec2 CurveEditor::snapToGrid(const Vec2& point) const {
    return Vec2(
        Math::round(point.x / m_gridSize) * m_gridSize,
        Math::round(point.y / m_gridSize) * m_gridSize
    );
}

bool CurveEditor::isPointSelected(int pointIndex) const {
    return m_selectedPointIndex == pointIndex;
}

void CurveEditor::handleInput() {
    // Handle mouse input for point selection, manipulation, etc.
    // This would be implemented based on the input system
}

void CurveEditor::updateSelection() {
    // Update selection based on current input state
}

void CurveEditor::updateCurves() {
    // Update curve interpolation based on point changes
    for (auto& curve : m_curves) {
        if (curve) {
            curve->update();
        }
    }
}

void CurveEditor::renderGrid() {
    if (!m_showGrid) {
        return;
    }
    
    // Render grid lines
    // This would be implemented using the rendering system
}

void CurveEditor::renderCurves() {
    // Render all curves
    for (auto& curve : m_curves) {
        if (curve) {
            renderCurve(curve);
        }
    }
}

void CurveEditor::renderCurve(AnimationCurve* curve) {
    if (!curve) {
        return;
    }
    
    // Set curve color based on selection
    bool isSelected = (curve == m_selectedCurve);
    // Render curve using the rendering system
    (void)isSelected;
}

void CurveEditor::renderPoints() {
    if (!m_selectedCurve) {
        return;
    }
    
    // Render control points for selected curve
    for (size_t i = 0; i < m_selectedCurve->getPointCount(); ++i) {
        bool isSelected = (i == static_cast<size_t>(m_selectedPointIndex));
        // Render point using the rendering system
        (void)isSelected;
    }
}

void CurveEditor::renderTangents() {
    if (!m_showTangents || !m_selectedCurve) {
        return;
    }
    
    // Render tangent handles for selected curve
    for (size_t i = 0; i < m_selectedCurve->getPointCount(); ++i) {
        Vec2 point = m_selectedCurve->getPoint(i);
        Vec2 tangent = m_selectedCurve->getTangent(i);
        
        Vec2 tangentEnd = point + tangent * m_tangentLength;
        
        // Render tangent line using the rendering system
        (void)point;
        (void)tangentEnd;
    }
}

void CurveEditor::renderUI() {
    // Render UI elements (toolbar, property panel, etc.)
}

} // namespace Animation
} // namespace RedFiles