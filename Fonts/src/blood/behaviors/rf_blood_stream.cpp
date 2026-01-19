#include "rf_blood_stream.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

BloodStreamBehavior::BloodStreamBehavior()
    : m_streamForce(5.0f)
    , m_streamRadius(0.3f)
    , m_streamLength(10.0f)
    , m_streamCount(0)
    , m_maxStreams(50)
    , m_isEnabled(true)
    , m_streamLifetime(3.0f)
    , m_streamFadeTime(0.5f)
    , m_streamWidth(0.2f)
    , m_streamColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_streamOpacity(0.8f)
    , m_streamSpeed(2.0f)
    , m_streamTurbulence(0.05f)
    , m_streamGravity(9.81f)
    , m_streamViscosity(0.8f)
    , m_streamCoagulation(0.1f)
{
    m_streams.reserve(m_maxStreams);
    RF_LOG_INFO("BloodStreamBehavior created");
}

BloodStreamBehavior::~BloodStreamBehavior() {
    clear();
    RF_LOG_INFO("BloodStreamBehavior destroyed");
}

void BloodStreamBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("BloodStreamBehavior initialized");
}

void BloodStreamBehavior::clear() {
    for (auto& stream : m_streams) {
        if (stream) {
            RF_DELETE(stream);
        }
    }
    
    m_streams.clear();
    m_streamCount = 0;
    RF_LOG_INFO("Cleared all blood streams");
}

void BloodStreamBehavior::addStream(const Vec3& position, const Vec3& velocity, float size, 
                           const Color& color, float lifetime) {
    if (m_streamCount >= m_maxStreams) {
        // Remove oldest stream
        RF_DELETE(m_streams.front());
        m_streams.erase(m_streams.begin());
        m_streamCount--;
    }
    
    auto stream = RF_NEW(BloodStream);
    stream->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_streams.push_back(stream);
    m_streamCount++;
    
    RF_LOG_INFO("Added blood stream at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void BloodStreamBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing streams
    for (auto& stream : m_streams) {
        if (stream) {
            stream->update(deltaTime);
            
            // Remove completed streams
            if (stream->isCompleted()) {
                RF_DELETE(stream);
                m_streams.erase(std::find(m_streams.begin(), m_streams.end(), stream));
                m_streamCount--;
            }
        }
    }
    
    // Add new streams based on blood flow
    updateStreamGeneration(deltaTime);
}

void BloodStreamBehavior::updateStreamGeneration(float deltaTime) {
    // Generate new streams based on blood flow
    // This would be connected to the blood flow system
    // For now, just update existing streams
    (void)deltaTime;
}

void BloodStreamBehavior::setStreamForce(float force) {
    m_streamForce = force;
    RF_LOG_INFO("Blood stream force set to %.2f", force);
}

void BloodStreamBehavior::setStreamRadius(float radius) {
    m_streamRadius = radius;
    RF_LOG_INFO("Blood stream radius set to %.2f", radius);
}

void BloodStreamBehavior::setStreamLength(float length) {
    m_streamLength = length;
    RF_LOG_INFO("Blood stream length set to %.2f", length);
}

void BloodStreamBehavior::setStreamCount(int count) {
    m_streamCount = count;
    RF_LOG_INFO("Blood stream count set to %d", count);
}

void BloodStreamBehavior::setStreamLifetime(float lifetime) {
    m_streamLifetime = lifetime;
    RF_LOG_INFO("Blood stream lifetime set to %.2f", lifetime);
}

void BloodStreamBehavior::setStreamFadeTime(float time) {
    m_streamFadeTime = time;
    RF_LOG_INFO("Blood stream fade time set to %.2f", time);
}

void BloodStreamBehavior::setStreamWidth(float width) {
    m_streamWidth = width;
    RF_LOG_INFO("Blood stream width set to %.2f", width);
}

void BloodStreamBehavior::setStreamColor(const Color& color) {
    m_streamColor = color;
    RF_LOG_INFO("Blood stream color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void BloodStreamBehavior::setStreamOpacity(float opacity) {
    m_streamOpacity = opacity;
    RF_LOG_INFO("Blood stream opacity set to %.2f", opacity);
}

void BloodStreamBehavior::setStreamSpeed(float speed) {
    m_streamSpeed = speed;
    RF_LOG_INFO("Blood stream speed set to %.2f", speed);
}

void BloodStreamBehavior::setStreamTurbulence(float turbulence) {
    m_streamTurbulence = turbulence;
    RF_LOG_INFO("Blood stream turbulence set to %.2f", turbulence);
}

void BloodStreamBehavior::setStreamGravity(float gravity) {
    m_streamGravity = gravity;
    RF_LOG_INFO("Blood stream gravity set to %.2f", gravity);
}

void BloodStreamBehavior::setStreamViscosity(float viscosity) {
    m_streamViscosity = viscosity;
    RF_LOG_INFO("Blood stream viscosity set to %.2f", viscosity);
}

void BloodStreamBehavior::setStreamCoagulation(float coagulation) {
    m_streamCoagulation = coagulation;
    RF_LOG_INFO("Blood stream coagulation set to %.2f", coagulation);
}

void BloodStreamBehavior::setMaxStreams(int maxStreams) {
    m_maxStreams = maxStreams;
    m_streams.reserve(maxStreams);
    RF_LOG_INFO("Blood stream max streams set to %d", maxStreams);
}

size_t BloodStreamBehavior::getStreamCount() const {
    return m_streamCount;
}

float BloodStreamBehavior::getStreamForce() const {
    return m_streamForce;
}

float BloodStreamBehavior::getStreamRadius() const {
    return m_streamRadius;
}

float BloodStreamBehavior::getStreamLength() const {
    return m_streamLength;
}

float BloodStreamBehavior::getStreamLifetime() const {
    return m_streamLifetime;
}

float BloodStreamBehavior::getStreamFadeTime() const {
    return m_streamFadeTime;
}

float BloodStreamBehavior::getStreamWidth() const {
    return m_streamWidth;
}

Color BloodStreamBehavior::getStreamColor() const {
    return m_streamColor;
}

float BloodStreamBehavior::getStreamOpacity() const {
    return m_streamOpacity;
}

float BloodStreamBehavior::getStreamSpeed() const {
    return m_streamSpeed;
}

float BloodStreamBehavior::getStreamTurbulence() const {
    return m_streamTurbulence;
}

float BloodStreamBehavior::getStreamGravity() const {
    return m_streamGravity;
}

float BloodStreamBehavior::getStreamViscosity() const {
    return m_streamViscosity;
}

float BloodStreamBehavior::getStreamCoagulation() const {
    return m_streamCoagulation;
}

bool BloodStreamBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<BloodStream*>& BloodStreamBehavior::getStreams() const {
    return m_streams;
}

void BloodStreamBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& BloodStreamBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles