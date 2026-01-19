#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace RFConfig {

// Configuration types
enum class ConfigType {
    SYSTEM,
    GRAPHICS,
    AUDIO,
    INPUT,
    NETWORK,
    DEBUG,
    PERFORMANCE,
    CUSTOM
};

// Configuration value types
enum class ValueType {
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    VECTOR2,
    VECTOR3,
    VECTOR4,
    COLOR,
    CUSTOM
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    std::string value;
    std::string defaultValue;
    ValueType type;
    std::string description;
    bool isRequired;
    bool isReadOnly;
    std::vector<std::string> allowedValues;
    std::map<std::string, std::any> metadata;
    
    ConfigEntry() : type(ValueType::STRING), isRequired(false), isReadOnly(false) {}
    ConfigEntry(const std::string& k, const std::string& v, const std::string& def, 
               ValueType t, const std::string& desc, bool req = false, bool readOnly = false)
        : key(k), value(v), defaultValue(def), type(t), description(desc), 
          isRequired(req), isReadOnly(readOnly) {}
};

// Configuration section
struct ConfigSection {
    std::string name;
    std::string description;
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::any> metadata;
    
    ConfigSection() {}
    ConfigSection(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
};

// Configuration manager
class ConfigManager {
public:
    ConfigManager();
    virtual ~ConfigManager() = default;
    
    // Configuration management
    void initialize();
    void reset();
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
    void loadFromString(const std::string& configData);
    std::string saveToString();
    
    // Section management
    void addSection(const ConfigSection& section);
    void removeSection(const std::string& sectionName);
    bool hasSection(const std::string& sectionName) const;
    ConfigSection getSection(const std::string& sectionName) const;
    std::vector<std::string> getSectionNames() const;
    
    // Entry management
    void addEntry(const std::string& sectionName, const ConfigEntry& entry);
    void removeEntry(const std::string& sectionName, const std::string& key);
    bool hasEntry(const std::string& sectionName, const std::string& key) const;
    ConfigEntry getEntry(const std::string& sectionName, const std::string& key) const;
    std::vector<ConfigEntry> getEntries(const std::string& sectionName) const;
    
    // Value access
    template<typename T>
    T getValue(const std::string& sectionName, const std::string& key, const T& defaultValue = T{}) const;
    
    template<typename T>
    void setValue(const std::string& sectionName, const std::string& key, const T& value);
    
    bool getBool(const std::string& sectionName, const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& sectionName, const std::string& key, int defaultValue = 0) const;
    float getFloat(const std::string& sectionName, const std::string& key, float defaultValue = 0.0f) const;
    std::string getString(const std::string& sectionName, const std::string& key, const std::string& defaultValue = "") const;
    
    void setBool(const std::string& sectionName, const std::string& key, bool value);
    void setInt(const std::string& sectionName, const std::string& key, int value);
    void setFloat(const std::string& sectionName, const std::string& key, float value);
    void setString(const std::string& sectionName, const std::string& key, const std::string& value);
    
    // Validation
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    bool validateEntry(const ConfigEntry& entry) const;
    
    // Event handling
    void addConfigEventListener(const std::string& eventType, std::function<void()> callback);
    void removeConfigEventListener(const std::string& eventType, std::function<void()> callback);
    void clearConfigEventListeners();
    
    // Utility methods
    void cloneFrom(const ConfigManager& other);
    virtual std::unique_ptr<ConfigManager> clone() const;
    
    // Data access
    const std::vector<ConfigSection>& getSections() const;
    const std::map<std::string, ConfigSection>& getSectionMap() const;
    
protected:
    // Protected members
    std::vector<ConfigSection> sections_;
    std::map<std::string, ConfigSection> sectionMap_;
    std::map<std::string, std::vector<std::function<void()>>> eventListeners_;
    
    // Protected helper methods
    virtual void triggerConfigEvent(const std::string& eventType);
    virtual void updateSectionMap();
    virtual void initializeDefaultSections();
    virtual void initializeSystemSection();
    virtual void initializeGraphicsSection();
    virtual void initializeAudioSection();
    virtual void initializeInputSection();
    virtual void initializeNetworkSection();
    virtual void initializeDebugSection();
    virtual void initializePerformanceSection();
    
    // File I/O helpers
    virtual bool parseConfigFile(const std::string& filename);
    virtual bool writeConfigFile(const std::string& filename);
    virtual bool parseConfigString(const std::string& configData);
    virtual std::string generateConfigString();
    
    // Value conversion helpers
    virtual std::string valueToString(const std::any& value, ValueType type) const;
    virtual std::any stringToValue(const std::string& str, ValueType type) const;
    virtual bool isValidValue(const std::string& value, const ConfigEntry& entry) const;
};

// Specialized configuration managers
class SystemConfig : public ConfigManager {
public:
    SystemConfig();
    void initializeSystemSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

class GraphicsConfig : public ConfigManager {
public:
    GraphicsConfig();
    void initializeGraphicsSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

class AudioConfig : public ConfigManager {
public:
    AudioConfig();
    void initializeAudioSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

class InputConfig : public ConfigManager {
public:
    InputConfig();
    void initializeInputSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

class NetworkConfig : public ConfigManager {
public:
    NetworkConfig();
    void initializeNetworkSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

class DebugConfig : public ConfigManager {
public:
    DebugConfig();
    void initializeDebugSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

class PerformanceConfig : public ConfigManager {
public:
    PerformanceConfig();
    void initializePerformanceSection() override;
    std::unique_ptr<ConfigManager> clone() const override;
};

// Configuration factory
class ConfigFactory {
public:
    static std::unique_ptr<ConfigManager> createConfig(ConfigType type);
    static std::unique_ptr<SystemConfig> createSystemConfig();
    static std::unique_ptr<GraphicsConfig> createGraphicsConfig();
    static std::unique_ptr<AudioConfig> createAudioConfig();
    static std::unique_ptr<InputConfig> createInputConfig();
    static std::unique_ptr<NetworkConfig> createNetworkConfig();
    static std::unique_ptr<DebugConfig> createDebugConfig();
    static std::unique_ptr<PerformanceConfig> createPerformanceConfig();
    static std::vector<ConfigType> getAvailableConfigTypes();
    static ConfigSection createDefaultSection(ConfigType type);
    static ConfigEntry createDefaultEntry(const std::string& key, ValueType type);
};

// Template implementations
template<typename T>
T ConfigManager::getValue(const std::string& sectionName, const std::string& key, const T& defaultValue) const {
    if (!hasEntry(sectionName, key)) {
        return defaultValue;
    }
    
    ConfigEntry entry = getEntry(sectionName, key);
    try {
        if constexpr (std::is_same_v<T, bool>) {
            return std::any_cast<bool>(stringToValue(entry.value, ValueType::BOOLEAN));
        } else if constexpr (std::is_same_v<T, int>) {
            return std::any_cast<int>(stringToValue(entry.value, ValueType::INTEGER));
        } else if constexpr (std::is_same_v<T, float>) {
            return std::any_cast<float>(stringToValue(entry.value, ValueType::FLOAT));
        } else if constexpr (std::is_same_v<T, std::string>) {
            return entry.value;
        }
    } catch (const std::exception&) {
        return defaultValue;
    }
    
    return defaultValue;
}

template<typename T>
void ConfigManager::setValue(const std::string& sectionName, const std::string& key, const T& value) {
    if (!hasSection(sectionName)) {
        addSection(ConfigSection(sectionName, ""));
    }
    
    ConfigEntry entry;
    entry.key = key;
    
    if constexpr (std::is_same_v<T, bool>) {
        entry.value = value ? "true" : "false";
        entry.type = ValueType::BOOLEAN;
    } else if constexpr (std::is_same_v<T, int>) {
        entry.value = std::to_string(value);
        entry.type = ValueType::INTEGER;
    } else if constexpr (std::is_same_v<T, float>) {
        entry.value = std::to_string(value);
        entry.type = ValueType::FLOAT;
    } else if constexpr (std::is_same_v<T, std::string>) {
        entry.value = value;
        entry.type = ValueType::STRING;
    }
    
    addEntry(sectionName, entry);
    triggerConfigEvent("value_changed");
}

} // namespace RFConfig