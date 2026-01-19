#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

namespace BloodEditor {

class PresetManager {
public:
    struct EffectPreset {
        std::string name;
        std::string description;
        std::vector<BloodParticle> particles;
        std::vector<BloodCurve> curves;
        std::map<std::string, float> parameters;
    };
    
    PresetManager();
    ~PresetManager() = default;
    
    bool loadPresets(const std::string& presetDirectory);
    bool savePreset(const EffectPreset& preset, const std::string& filename);
    bool loadPreset(const std::string& filename, EffectPreset& preset);
    
    void addPreset(const EffectPreset& preset);
    void removePreset(const std::string& name);
    
    const std::vector<EffectPreset>& getPresets() const { return presets_; }
    std::vector<std::string> getPresetNames() const;
    
    EffectPreset* findPreset(const std::string& name);
    const EffectPreset* findPreset(const std::string& name) const;
    
    void createDefaultPresets();
    
    bool applyPreset(BloodEffect* effect, const std::string& presetName);
    EffectPreset createPresetFromEffect(const BloodEffect* effect, const std::string& name, const std::string& description = "");
    
private:
    std::vector<EffectPreset> presets_;
    std::string presetDirectory_;
    
    std::string presetToJSON(const EffectPreset& preset) const;
    EffectPreset presetFromJSON(const std::string& json) const;
    
    std::string particleToJSON(const BloodParticle& particle) const;
    std::string curveToJSON(const BloodCurve& curve) const;
    
    BloodParticle particleFromJSON(const std::string& json) const;
    BloodCurve curveFromJSON(const std::string& json) const;
};

PresetManager::PresetManager() {
    createDefaultPresets();
}

bool PresetManager::loadPresets(const std::string& presetDirectory) {
    presetDirectory_ = presetDirectory;
    // Implementation would scan directory and load all .preset files
    std::cout << "Loading presets from: " << presetDirectory << std::endl;
    return true;
}

bool PresetManager::savePreset(const EffectPreset& preset, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open preset file for writing: " << filename << std::endl;
        return false;
    }
    
    file << presetToJSON(preset);
    file.close();
    
    std::cout << "Saved preset: " << preset.name << " to " << filename << std::endl;
    return true;
}

bool PresetManager::loadPreset(const std::string& filename, EffectPreset& preset) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open preset file: " << filename << std::endl;
        return false;
    }
    
    std::string json((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();
    
    preset = presetFromJSON(json);
    std::cout << "Loaded preset: " << preset.name << " from " << filename << std::endl;
    return true;
}

void PresetManager::addPreset(const EffectPreset& preset) {
    // Remove existing preset with same name if it exists
    removePreset(preset.name);
    presets_.push_back(preset);
}

void PresetManager::removePreset(const std::string& name) {
    presets_.erase(
        std::remove_if(presets_.begin(), presets_.end(),
            [&name](const EffectPreset& p) { return p.name == name; }),
        presets_.end()
    );
}

std::vector<std::string> PresetManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : presets_) {
        names.push_back(preset.name);
    }
    return names;
}

EffectPreset* PresetManager::findPreset(const std::string& name) {
    auto it = std::find_if(presets_.begin(), presets_.end(),
        [&name](const EffectPreset& p) { return p.name == name; });
    return (it != presets_.end()) ? &(*it) : nullptr;
}

const EffectPreset* PresetManager::findPreset(const std::string& name) const {
    auto it = std::find_if(presets_.begin(), presets_.end(),
        [&name](const EffectPreset& p) { return p.name == name; });
    return (it != presets_.end()) ? &(*it) : nullptr;
}

void PresetManager::createDefaultPresets() {
    // Blood Splash Preset
    EffectPreset bloodSplash;
    bloodSplash.name = "Blood Splash";
    bloodSplash.description = "Intense blood splash effect with multiple particles";
    
    for (int i = 0; i < 50; ++i) {
        BloodParticle particle;
        particle.position = glm::vec2(0, 0);
        particle.velocity = glm::vec2(
            (rand() % 200 - 100) / 10.0f,
            (rand() % 200 - 50) / 10.0f
        );
        particle.color = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
        particle.size = (rand() % 20 + 5) / 10.0f;
        particle.lifetime = 2.0f + (rand() % 300) / 100.0f;
        particle.age = 0.0f;
        particle.active = true;
        bloodSplash.particles.push_back(particle);
    }
    
    bloodSplash.parameters["gravity"] = 98.0f;
    bloodSplash.parameters["drag"] = 0.99f;
    bloodSplash.parameters["emission_rate"] = 50.0f;
    
    // Blood Drip Preset
    EffectPreset bloodDrip;
    bloodDrip.name = "Blood Drip";
    bloodDrip.description = "Slow dripping blood effect";
    
    for (int i = 0; i < 20; ++i) {
        BloodParticle particle;
        particle.position = glm::vec2((rand() % 100 - 50) / 10.0f, 50.0f);
        particle.velocity = glm::vec2(
            (rand() % 20 - 10) / 100.0f,
            (rand() % 50 + 20) / 100.0f
        );
        particle.color = glm::vec4(0.9f, 0.2f, 0.2f, 1.0f);
        particle.size = (rand() % 15 + 5) / 10.0f;
        particle.lifetime = 5.0f + (rand() % 500) / 100.0f;
        particle.age = 0.0f;
        particle.active = true;
        bloodDrip.particles.push_back(particle);
    }
    
    bloodDrip.parameters["gravity"] = 98.0f;
    bloodDrip.parameters["drag"] = 0.98f;
    bloodDrip.parameters["emission_rate"] = 5.0f;
    
    // Blood Spray Preset
    EffectPreset bloodSpray;
    bloodSpray.name = "Blood Spray";
    bloodSpray.description = "Directional blood spray effect";
    
    for (int i = 0; i < 30; ++i) {
        BloodParticle particle;
        particle.position = glm::vec2(0, 0);
        particle.velocity = glm::vec2(
            (rand() % 100 + 50) / 10.0f,  // Mostly rightward
            (rand() % 100 - 50) / 10.0f
        );
        particle.color = glm::vec4(0.7f, 0.05f, 0.05f, 1.0f);
        particle.size = (rand() % 10 + 3) / 10.0f;
        particle.lifetime = 1.5f + (rand() % 200) / 100.0f;
        particle.age = 0.0f;
        particle.active = true;
        bloodSpray.particles.push_back(particle);
    }
    
    bloodSpray.parameters["gravity"] = 49.0f;
    bloodSpray.parameters["drag"] = 0.95f;
    bloodSpray.parameters["emission_rate"] = 30.0f;
    
    // Add presets to manager
    addPreset(bloodSplash);
    addPreset(bloodDrip);
    addPreset(bloodSpray);
    
    std::cout << "Created " << presets_.size() << " default presets" << std::endl;
}

bool PresetManager::applyPreset(BloodEffect* effect, const std::string& presetName) {
    if (!effect) return false;
    
    const EffectPreset* preset = findPreset(presetName);
    if (!preset) {
        std::cerr << "Preset not found: " << presetName << std::endl;
        return false;
    }
    
    // Clear existing effect
    effect->clear();
    
    // Add particles from preset
    for (const auto& particle : preset->particles) {
        effect->addParticle(particle);
    }
    
    // Add curves from preset
    for (const auto& curve : preset->curves) {
        effect->addCurve(curve);
    }
    
    std::cout << "Applied preset: " << presetName << std::endl;
    return true;
}

EffectPreset PresetManager::createPresetFromEffect(const BloodEffect* effect, const std::string& name, const std::string& description) {
    EffectPreset preset;
    preset.name = name;
    preset.description = description;
    
    if (effect) {
        preset.particles = effect->getParticles();
        preset.curves = effect->getCurves();
    }
    
    return preset;
}

std::string PresetManager::presetToJSON(const EffectPreset& preset) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << preset.name << "\",\n";
    oss << "  \"description\": \"" << preset.description << "\",\n";
    oss << "  \"parameters\": {\n";
    
    bool first = true;
    for (const auto& param : preset.parameters) {
        if (!first) oss << ",\n";
        oss << "    \"" << param.first << "\": " << param.second;
        first = false;
    }
    oss << "\n  },\n";
    
    oss << "  \"particles\": [\n";
    for (size_t i = 0; i < preset.particles.size(); ++i) {
        oss << "    " << particleToJSON(preset.particles[i]);
        if (i < preset.particles.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    oss << "  \"curves\": [\n";
    for (size_t i = 0; i < preset.curves.size(); ++i) {
        oss << "    " << curveToJSON(preset.curves[i]);
        if (i < preset.curves.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}";
    
    return oss.str();
}

EffectPreset PresetManager::presetFromJSON(const std::string& json) const {
    // Simplified JSON parsing - in a real implementation, use a proper JSON library
    EffectPreset preset;
    preset.name = "Unknown";
    preset.description = "Loaded from JSON";
    
    // This would be a proper JSON parser in a real implementation
    std::cout << "JSON parsing would be implemented here" << std::endl;
    
    return preset;
}

std::string PresetManager::particleToJSON(const BloodParticle& particle) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "      \"position\": [" << particle.position.x << ", " << particle.position.y << "],\n";
    oss << "      \"velocity\": [" << particle.velocity.x << ", " << particle.velocity.y << "],\n";
    oss << "      \"color\": [" << particle.color.r << ", " << particle.color.g << ", " << particle.color.b << ", " << particle.color.w << "],\n";
    oss << "      \"size\": " << particle.size << ",\n";
    oss << "      \"lifetime\": " << particle.lifetime << ",\n";
    oss << "      \"age\": " << particle.age << ",\n";
    oss << "      \"active\": " << (particle.active ? "true" : "false");
    oss << "\n    }";
    return oss.str();
}

std::string PresetManager::curveToJSON(const BloodCurve& curve) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "      \"name\": \"" << curve.name << "\",\n";
    oss << "      \"thickness\": " << curve.thickness << ",\n";
    oss << "      \"color\": [" << curve.color.r << ", " << curve.color.g << ", " << curve.color.b << ", " << curve.color.a << "],\n";
    oss << "      \"points\": [";
    for (size_t i = 0; i < curve.points.size(); ++i) {
        oss << "[" << curve.points[i].x << ", " << curve.points[i].y << "]";
        if (i < curve.points.size() - 1) oss << ", ";
    }
    oss << "]\n    }";
    return oss.str();
}

BloodParticle PresetManager::particleFromJSON(const std::string& json) const {
    BloodParticle particle;
    // JSON parsing implementation would go here
    return particle;
}

BloodCurve PresetManager::curveFromJSON(const std::string& json) const {
    BloodCurve curve;
    // JSON parsing implementation would go here
    return curve;
}

} // namespace BloodEditor