#include "editor.h"
#include "ui_components.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace BloodEditor {

class ExportManager {
public:
    enum class ExportFormat {
        JSON,
        BINARY,
        XML,
        CUSTOM
    };
    
    struct ExportSettings {
        ExportFormat format = ExportFormat::JSON;
        bool compressData = false;
        bool includeMetadata = true;
        std::string customExtension = ".blood";
        int version = 1;
    };
    
    ExportManager();
    ~ExportManager() = default;
    
    bool exportEffect(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings);
    bool importEffect(BloodEffect* effect, const std::string& filename);
    
    void setExportSettings(const ExportSettings& settings) { exportSettings_ = settings; }
    const ExportSettings& getExportSettings() const { return exportSettings_; }
    
    std::vector<std::string> getSupportedFormats() const;
    std::string getFileExtension(ExportFormat format) const;
    
private:
    ExportSettings exportSettings_;
    
    bool exportAsJSON(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings);
    bool exportAsBinary(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings);
    bool exportAsXML(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings);
    
    bool importFromJSON(BloodEffect* effect, const std::string& filename);
    bool importFromBinary(BloodEffect* effect, const std::string& filename);
    bool importFromXML(BloodEffect* effect, const std::string& filename);
    
    std::string particleToJSON(const BloodParticle& particle) const;
    std::string curveToJSON(const BloodCurve& curve) const;
    std::string vec4ToJSON(const glm::vec4& vec) const;
    std::string vec2ToJSON(const glm::vec2& vec) const;
    
    void writeBinaryParticle(std::ofstream& file, const BloodParticle& particle) const;
    void writeBinaryCurve(std::ofstream& file, const BloodCurve& curve) const;
    
    BloodParticle readBinaryParticle(std::ifstream& file) const;
    BloodCurve readBinaryCurve(std::ifstream& file) const;
};

ExportManager::ExportManager() {
}

bool ExportManager::exportEffect(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings) {
    if (!effect) return false;
    
    switch (settings.format) {
        case ExportFormat::JSON:
            return exportAsJSON(effect, filename, settings);
        case ExportFormat::BINARY:
            return exportAsBinary(effect, filename, settings);
        case ExportFormat::XML:
            return exportAsXML(effect, filename, settings);
        case ExportFormat::CUSTOM:
            // Custom format would be implemented here
            std::cerr << "Custom export format not implemented" << std::endl;
            return false;
        default:
            return false;
    }
}

bool ExportManager::importEffect(BloodEffect* effect, const std::string& filename) {
    if (!effect) return false;
    
    // Determine format from file extension
    std::string extension = filename.substr(filename.find_last_of('.') + 1);
    
    if (extension == "json") {
        return importFromJSON(effect, filename);
    } else if (extension == "bin" || extension == "blood") {
        return importFromBinary(effect, filename);
    } else if (extension == "xml") {
        return importFromXML(effect, filename);
    }
    
    std::cerr << "Unsupported file format: " << extension << std::endl;
    return false;
}

std::vector<std::string> ExportManager::getSupportedFormats() const {
    return {"JSON (.json)", "Binary (.bin, .blood)", "XML (.xml)"};
}

std::string ExportManager::getFileExtension(ExportFormat format) const {
    switch (format) {
        case ExportFormat::JSON: return ".json";
        case ExportFormat::BINARY: return exportSettings_.customExtension;
        case ExportFormat::XML: return ".xml";
        case ExportFormat::CUSTOM: return exportSettings_.customExtension;
        default: return ".blood";
    }
}

bool ExportManager::exportAsJSON(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "{\n";
    
    if (settings.includeMetadata) {
        file << "  \"metadata\": {\n";
        file << "    \"version\": " << settings.version << ",\n";
        file << "    \"exported\": \"blood_editor\",\n";
        file << "    \"format\": \"json\"\n";
        file << "  },\n";
    }
    
    // Export particles
    file << "  \"particles\": [\n";
    const auto& particles = effect->getParticles();
    for (size_t i = 0; i < particles.size(); ++i) {
        file << "    " << particleToJSON(particles[i]);
        if (i < particles.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ],\n";
    
    // Export curves
    file << "  \"curves\": [\n";
    const auto& curves = effect->getCurves();
    for (size_t i = 0; i < curves.size(); ++i) {
        file << "    " << curveToJSON(curves[i]);
        if (i < curves.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ]\n";
    
    file << "}\n";
    
    file.close();
    std::cout << "Successfully exported effect to " << filename << std::endl;
    return true;
}

bool ExportManager::exportAsBinary(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Write header
    uint32_t magic = 0x424C4F44; // "BLOD" in hex
    uint32_t version = settings.version;
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Write particle count
    const auto& particles = effect->getParticles();
    uint32_t particleCount = static_cast<uint32_t>(particles.size());
    file.write(reinterpret_cast<const char*>(&particleCount), sizeof(particleCount));
    
    // Write particles
    for (const auto& particle : particles) {
        writeBinaryParticle(file, particle);
    }
    
    // Write curve count
    const auto& curves = effect->getCurves();
    uint32_t curveCount = static_cast<uint32_t>(curves.size());
    file.write(reinterpret_cast<const char*>(&curveCount), sizeof(curveCount));
    
    // Write curves
    for (const auto& curve : curves) {
        writeBinaryCurve(file, curve);
    }
    
    file.close();
    std::cout << "Successfully exported effect to " << filename << std::endl;
    return true;
}

bool ExportManager::exportAsXML(const BloodEffect* effect, const std::string& filename, const ExportSettings& settings) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<blood_effect version=\"" << settings.version << "\">\n";
    
    if (settings.includeMetadata) {
        file << "  <metadata>\n";
        file << "    <exporter>blood_editor</exporter>\n";
        file << "    <format>xml</format>\n";
        file << "  </metadata>\n";
    }
    
    // Export particles
    file << "  <particles>\n";
    const auto& particles = effect->getParticles();
    for (const auto& particle : particles) {
        file << "    <particle>\n";
        file << "      <position>" << vec2ToJSON(particle.position) << "</position>\n";
        file << "      <velocity>" << vec2ToJSON(particle.velocity) << "</velocity>\n";
        file << "      <color>" << vec4ToJSON(particle.color) << "</color>\n";
        file << "      <size>" << particle.size << "</size>\n";
        file << "      <lifetime>" << particle.lifetime << "</lifetime>\n";
        file << "      <age>" << particle.age << "</age>\n";
        file << "      <active>" << (particle.active ? "true" : "false") << "</active>\n";
        file << "    </particle>\n";
    }
    file << "  </particles>\n";
    
    // Export curves
    file << "  <curves>\n";
    const auto& curves = effect->getCurves();
    for (const auto& curve : curves) {
        file << "    <curve name=\"" << curve.name << "\">\n";
        file << "      <thickness>" << curve.thickness << "</thickness>\n";
        file << "      <color>" << vec4ToJSON(curve.color) << "</color>\n";
        file << "      <points>\n";
        for (const auto& point : curve.points) {
            file << "        <point>" << vec2ToJSON(point) << "</point>\n";
        }
        file << "      </points>\n";
        file << "    </curve>\n";
    }
    file << "  </curves>\n";
    
    file << "</blood_effect>\n";
    
    file.close();
    std::cout << "Successfully exported effect to " << filename << std::endl;
    return true;
}

std::string ExportManager::particleToJSON(const BloodParticle& particle) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "      \"position\": " << vec2ToJSON(particle.position) << ",\n";
    oss << "      \"velocity\": " << vec2ToJSON(particle.velocity) << ",\n";
    oss << "      \"color\": " << vec4ToJSON(particle.color) << ",\n";
    oss << "      \"size\": " << particle.size << ",\n";
    oss << "      \"lifetime\": " << particle.lifetime << ",\n";
    oss << "      \"age\": " << particle.age << ",\n";
    oss << "      \"active\": " << (particle.active ? "true" : "false");
    oss << "\n    }";
    return oss.str();
}

std::string ExportManager::curveToJSON(const BloodCurve& curve) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "      \"name\": \"" << curve.name << "\",\n";
    oss << "      \"thickness\": " << curve.thickness << ",\n";
    oss << "      \"color\": " << vec4ToJSON(curve.color) << ",\n";
    oss << "      \"points\": [";
    for (size_t i = 0; i < curve.points.size(); ++i) {
        oss << vec2ToJSON(curve.points[i]);
        if (i < curve.points.size() - 1) oss << ", ";
    }
    oss << "]\n    }";
    return oss.str();
}

std::string ExportManager::vec4ToJSON(const glm::vec4& vec) const {
    std::ostringstream oss;
    oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]";
    return oss.str();
}

std::string ExportManager::vec2ToJSON(const glm::vec2& vec) const {
    std::ostringstream oss;
    oss << "[" << vec.x << ", " << vec.y << "]";
    return oss.str();
}

void ExportManager::writeBinaryParticle(std::ofstream& file, const BloodParticle& particle) const {
    file.write(reinterpret_cast<const char*>(&particle.position), sizeof(particle.position));
    file.write(reinterpret_cast<const char*>(&particle.velocity), sizeof(particle.velocity));
    file.write(reinterpret_cast<const char*>(&particle.color), sizeof(particle.color));
    file.write(reinterpret_cast<const char*>(&particle.size), sizeof(particle.size));
    file.write(reinterpret_cast<const char*>(&particle.lifetime), sizeof(particle.lifetime));
    file.write(reinterpret_cast<const char*>(&particle.age), sizeof(particle.age));
    file.write(reinterpret_cast<const char*>(&particle.active), sizeof(particle.active));
}

void ExportManager::writeBinaryCurve(std::ofstream& file, const BloodCurve& curve) const {
    // Write name length and name
    uint32_t nameLength = static_cast<uint32_t>(curve.name.length());
    file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    file.write(curve.name.c_str(), nameLength);
    
    // Write curve data
    file.write(reinterpret_cast<const char*>(&curve.thickness), sizeof(curve.thickness));
    file.write(reinterpret_cast<const char*>(&curve.color), sizeof(curve.color));
    
    // Write points
    uint32_t pointCount = static_cast<uint32_t>(curve.points.size());
    file.write(reinterpret_cast<const char*>(&pointCount), sizeof(pointCount));
    for (const auto& point : curve.points) {
        file.write(reinterpret_cast<const char*>(&point), sizeof(point));
    }
}

bool ExportManager::importFromJSON(BloodEffect* effect, const std::string& filename) {
    // JSON import implementation would go here
    std::cout << "JSON import not yet implemented" << std::endl;
    return false;
}

bool ExportManager::importFromBinary(BloodEffect* effect, const std::string& filename) {
    // Binary import implementation would go here
    std::cout << "Binary import not yet implemented" << std::endl;
    return false;
}

bool ExportManager::importFromXML(BloodEffect* effect, const std::string& filename) {
    // XML import implementation would go here
    std::cout << "XML import not yet implemented" << std::endl;
    return false;
}

BloodParticle ExportManager::readBinaryParticle(std::ifstream& file) const {
    BloodParticle particle;
    file.read(reinterpret_cast<char*>(&particle.position), sizeof(particle.position));
    file.read(reinterpret_cast<char*>(&particle.velocity), sizeof(particle.velocity));
    file.read(reinterpret_cast<char*>(&particle.color), sizeof(particle.color));
    file.read(reinterpret_cast<char*>(&particle.size), sizeof(particle.size));
    file.read(reinterpret_cast<char*>(&particle.lifetime), sizeof(particle.lifetime));
    file.read(reinterpret_cast<char*>(&particle.age), sizeof(particle.age));
    file.read(reinterpret_cast<char*>(&particle.active), sizeof(particle.active));
    return particle;
}

BloodCurve ExportManager::readBinaryCurve(std::ifstream& file) const {
    BloodCurve curve;
    
    // Read name
    uint32_t nameLength;
    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    curve.name.resize(nameLength);
    file.read(&curve.name[0], nameLength);
    
    // Read curve data
    file.read(reinterpret_cast<char*>(&curve.thickness), sizeof(curve.thickness));
    file.read(reinterpret_cast<char*>(&curve.color), sizeof(curve.color));
    
    // Read points
    uint32_t pointCount;
    file.read(reinterpret_cast<char*>(&pointCount), sizeof(pointCount));
    curve.points.resize(pointCount);
    for (uint32_t i = 0; i < pointCount; ++i) {
        file.read(reinterpret_cast<char*>(&curve.points[i]), sizeof(curve.points[i]));
    }
    
    return curve;
}

} // namespace BloodEditor