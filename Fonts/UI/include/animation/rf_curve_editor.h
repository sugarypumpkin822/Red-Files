#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace RFAnimation {

// Curve types
enum class CurveType {
    LINEAR,
    BEZIER,
    CATMULL_ROM,
    B_SPLINE,
    HERMITE,
    STEP,
    CONSTANT
};

// Control point types
enum class ControlPointType {
    ANCHOR,
    CONTROL,
    BREAKPOINT
    TANGENT
    SMOOTH
};

// Control point structure
struct ControlPoint {
    float x, y;
    float inX, inY, outX, outY;
    ControlPointType type;
    bool selected;
    int curveIndex;
    
    ControlPoint() : x(0.0f), y(0.0f), inX(0.0f), inY(0.0f), outX(0.0f), outY(0.0f), 
                 type(ControlPointType::ANCHOR), selected(false), curveIndex(-1) {}
    ControlPoint(float x_, float y_, float inX_, float inY_, float outX_, float outY_, 
               ControlPointType type_, bool selected_ = false, int curveIndex_ = -1)
        : x(x_), y(y_), inX(inX_), inY(inY_), outX(outX_), outY(outY_), 
          type(type_), selected(selected_), curveIndex(curveIndex_) {}
};

// Curve segment
struct CurveSegment {
    std::vector<ControlPoint> points;
    CurveType type;
    float startT, endT;
    int curveIndex;
    
    CurveSegment() : type(CurveType::LINEAR), startT(0.0f), endT(1.0f), curveIndex(-1) {}
    CurveSegment(CurveType type_, float startT_, float endT_, int curveIndex_)
        : type(type_), startT(startT_), endT(endT_), curveIndex(curveIndex_) {}
};

// Curve editor class
class CurveEditor {
public:
    CurveEditor();
    ~CurveEditor() = default;
    
    // Curve management
    void addCurve(const std::string& name, const std::vector<ControlPoint>& points, 
                CurveType type = CurveType::BEZIER);
    void removeCurve(const std::string& name);
    void clearCurve(const std::string& name);
    std::shared_ptr<CurveSegment> getCurve(const std::string& name) const;
    const std::map<std::string, std::shared_ptr<CurveSegment>>& getCurves() const;
    const std::vector<std::string>& getCurveNames() const;
    
    // Control point manipulation
    void addControlPoint(const std::string& curveName, const ControlPoint& point);
    void removeControlPoint(const std::string& curveName, int pointIndex);
    void moveControlPoint(const std::string& curveName, int pointIndex, float x, float y);
    void setControlPointType(const std::string& curveName, int pointIndex, ControlPointType type);
    void setControlPointTangents(const std::string& curveName, int pointIndex, 
                         float inX, float inY, float outX, float outY);
    
    // Curve editing operations
    void smoothCurve(const std::string& name);
    void simplifyCurve(const std::string& name, float tolerance = 0.1f);
    void subdivideCurve(const std::string& name, int segments = 2);
    void reverseCurve(const std::string& name);
    void mirrorCurve(const std::string& name, bool horizontal = true, bool vertical = false);
    void optimizeCurve(const std::string& name);
    
    // Curve evaluation
    std::vector<ControlPoint> evaluateCurve(const std::string& name, float startT, float endT, int numPoints = 100) const;
    float evaluateCurveAt(const std::string& name, float t) const;
    float getCurveLength(const std::string& name) const;
    float getCurveArea(const std::string& name) const;
    std::pair<float, float> getCurveBounds(const std::string& name) const;
    
    // Curve conversion
    void convertCurveType(const std::string& name, CurveType newType);
    void convertToLinear(const std::string& name);
    void convertToBezier(const std::string& name);
    void convertToCatmullRom(const std::string& name);
    void convertToBSpline(const std::string& name);
    void convertToHermite(const std::string& name);
    
    // Presets
    void addPreset(const std::string& name, const std::vector<ControlPoint>& points, CurveType type);
    void removePreset(const std::string& name);
    std::vector<std::string> getPresetNames() const;
    void loadPreset(const std::string& name);
    void savePreset(const std::string& name, const std::vector<ControlPoint>& points, CurveType type);
    
    // Import/Export
    void importCurve(const std::string& filename);
    void exportCurve(const std::string& filename, const std::string& curveName);
    void exportAllCurves(const std::string& filename);
    
    // Utility methods
    void cloneFrom(const CurveEditor& other);
    std::unique_ptr<CurveEditor> clone() const;
    
    // Data access
    const std::map<std::string, std::shared_ptr<CurveSegment>>& getCurves() const;
    const std::map<std::string, std::vector<ControlPoint>>& getPresetPoints() const;
    const std::map<std::string, CurveType>& getPresetTypes() const;
    
protected:
    // Protected members
    std::map<std::string, std::shared_ptr<CurveSegment>> curves_;
    std::map<std::string, std::vector<ControlPoint>> presetPoints_;
    std::map<std::string, CurveType> presetTypes_;
    
    // Protected helper methods
    virtual void updateCurve(const std::string& name);
    virtual void updateControlPoint(const std::string& curveName, int pointIndex);
    virtual void updateCurveSegment(const std::string& name);
    virtual void calculateTangents(const std::string& curveName);
    virtual void optimizeCurveSegment(const std::string& name);
    
    // Curve evaluation helpers
    virtual float evaluateLinear(const std::vector<ControlPoint>& points, float t) const;
    virtual float evaluateBezier(const std::vector<ControlPoint>& points, float t) const;
    virtual float evaluateCatmullRom(const std::vector<ControlPoint>& points, float t) const;
    virtual float evaluateBSpline(const std::vector<ControlPoint>& points, float t) const;
    virtual float evaluateHermite(const std::vector<ControlParam>& points, float t) const;
    
    // Utility helpers
    virtual void calculateCurveLength(const std::string& name);
    virtual void calculateCurveArea(const std::string& name);
    virtual void calculateCurveBounds(const std::string& name);
    virtual void validateCurve(const std::string& name);
    virtual void validateControlPoint(const std::string& curveName, const ControlPoint& point);
    
    // Preset management
    void loadDefaultPresets();
    void saveDefaultPresets();
    std::string getPresetFilePath() const;
};

// Specialized curve classes
class LinearCurve : public CurveSegment {
public:
    LinearCurve(const std::vector<ControlPoint>& points, float startT, float endT, int curveIndex);
    float evaluateAt(float t) const override;
    std::vector<ControlPoint> evaluate(float startT, float endT, int numPoints) const override;
};

class BezierCurve : public CurveSegment {
public:
    BezierCurve(const std::vector<ControlPoint>& points, float startT, float endT, int curveIndex);
    float evaluateAt(float t) const override;
    std::vector<ControlPoint> evaluate(float startT, float endT, int numPoints) const override;
};

class CatmullRomCurve : public CurveSegment {
public:
    CatmullRomCurve(const std::vector<ControlPoint>& points, float startT, float endT, int curveIndex);
    float evaluateAt(float t) const override;
    std::vector<ControlPoint> evaluate(float startT, float endT, int numPoints) const override;
};

class BSplineCurve : public CurveSegment {
public:
    BSplineCurve(const std::vector<ControlPoint>& points, float startT, float endT, int curveIndex);
    float evaluateAt(float t) const override;
    std::vector<ControlPoint> evaluate(float startT, float endT, int numPoints) const override;
};

class HermiteCurve : public CurveSegment {
public:
    HermiteCurve(const std::vector<ControlPoint>& points, float startT, float endT, int curveIndex);
    float evaluateAt(float t) const override;
    std::vector<ControlPoint> evaluate(float startT, float endT, int numPoints) const override;
};

} // namespace RFAnimation