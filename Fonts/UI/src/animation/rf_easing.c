#include "rf_easing.h"
#include "../math/rf_math.h"

namespace RedFiles {
namespace Animation {

// Linear easing
float linear(float t) {
    return t;
}

// Quadratic easing functions
float easeInQuad(float t) {
    return t * t;
}

float easeOutQuad(float t) {
    return t * (2.0f - t);
}

float easeInOutQuad(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    }
    return -1.0f + (4.0f - 2.0f * t) * t;
}

// Cubic easing functions
float easeInCubic(float t) {
    return t * t * t;
}

float easeOutCubic(float t) {
    float p = t - 1.0f;
    return p * p * p + 1.0f;
}

float easeInOutCubic(float t) {
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    }
    float p = 2.0f * t - 2.0f;
    return 1.0f + p * p * p * 0.5f;
}

// Quartic easing functions
float easeInQuart(float t) {
    return t * t * t * t;
}

float easeOutQuart(float t) {
    float p = t - 1.0f;
    return 1.0f - p * p * p * p;
}

float easeInOutQuart(float t) {
    if (t < 0.5f) {
        return 8.0f * t * t * t * t;
    }
    float p = t - 1.0f;
    return 1.0f - 8.0f * p * p * p * p;
}

// Quintic easing functions
float easeInQuint(float t) {
    return t * t * t * t * t;
}

float easeOutQuint(float t) {
    float p = t - 1.0f;
    return 1.0f + p * p * p * p * p;
}

float easeInOutQuint(float t) {
    if (t < 0.5f) {
        return 16.0f * t * t * t * t * t;
    }
    float p = 2.0f * t - 2.0f;
    return 1.0f + p * p * p * p * p * 0.5f;
}

// Sine easing functions
float easeInSine(float t) {
    return 1.0f - Math::cos(t * Math::PI * 0.5f);
}

float easeOutSine(float t) {
    return Math::sin(t * Math::PI * 0.5f);
}

float easeInOutSine(float t) {
    return -0.5f * (Math::cos(Math::PI * t) - 1.0f);
}

// Exponential easing functions
float easeInExpo(float t) {
    return (t == 0.0f) ? 0.0f : Math::pow(2.0f, 10.0f * (t - 1.0f));
}

float easeOutExpo(float t) {
    return (t == 1.0f) ? 1.0f : 1.0f - Math::pow(2.0f, -10.0f * t);
}

float easeInOutExpo(float t) {
    if (t == 0.0f || t == 1.0f) {
        return t;
    }
    
    if (t < 0.5f) {
        return 0.5f * Math::pow(2.0f, 20.0f * t - 10.0f);
    }
    
    return 1.0f - 0.5f * Math::pow(2.0f, -20.0f * t + 10.0f);
}

// Circular easing functions
float easeInCirc(float t) {
    return 1.0f - Math::sqrt(1.0f - t * t);
}

float easeOutCirc(float t) {
    float p = t - 1.0f;
    return Math::sqrt(1.0f - p * p);
}

float easeInOutCirc(float t) {
    if (t < 0.5f) {
        return 0.5f * (1.0f - Math::sqrt(1.0f - 4.0f * t * t));
    }
    
    return 0.5f * (Math::sqrt(1.0f - 4.0f * (t - 1.0f) * (t - 1.0f)) + 1.0f);
}

// Back easing functions
float easeInBack(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1.0f;
    float c2 = c1 * 1.525f;
    
    return c3 * t * t * t - c2 * t * t;
}

float easeOutBack(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1.0f;
    float c2 = c1 * 1.525f;
    
    float p = t - 1.0f;
    return 1.0f + c3 * p * p * p + c2 * p * p;
}

float easeInOutBack(float t) {
    float c1 = 1.70158f;
    float c2 = c1 * 1.525f;
    float c3 = c1 + 1.0f;
    
    if (t < 0.5f) {
        float p = 2.0f * t;
        return (p * p * ((c3 + 1.0f) * p - c2)) * 0.5f;
    }
    
    float p = 2.0f * t - 2.0f;
    return 1.0f + 0.5f * (p * p * ((c3 + 1.0f) * p + c2) + 2.0f);
}

// Elastic easing functions
float easeInElastic(float t) {
    if (t == 0.0f || t == 1.0f) {
        return t;
    }
    
    float p = 0.3f;
    return -Math::pow(2.0f, 10.0f * (t - 1.0f)) * Math::sin((t - 1.0f - p / 4.0f) * (2.0f * Math::PI) / p);
}

float easeOutElastic(float t) {
    if (t == 0.0f || t == 1.0f) {
        return t;
    }
    
    float p = 0.3f;
    return Math::pow(2.0f, -10.0f * t) * Math::sin((t - p / 4.0f) * (2.0f * Math::PI) / p) + 1.0f;
}

float easeInOutElastic(float t) {
    if (t == 0.0f || t == 1.0f) {
        return t;
    }
    
    float p = 0.3f * 1.5f;
    
    if (t < 0.5f) {
        float s = p / 4.0f;
        return -0.5f * (Math::pow(2.0f, 20.0f * (t - 1.0f)) * Math::sin((20.0f * t - 1.0f - s) * (2.0f * Math::PI) / p));
    }
    
    return Math::pow(2.0f, -20.0f * (t - 1.0f)) * Math::sin((20.0f * t - 1.0f - p / 4.0f) * (2.0f * Math::PI) / p) * 0.5f + 1.0f;
}

// Bounce easing functions
float easeOutBounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    }
    
    t -= 2.625f / 2.75f;
    return 7.5625f * t * t + 0.984375f;
}

float easeInBounce(float t) {
    return 1.0f - easeOutBounce(1.0f - t);
}

float easeInOutBounce(float t) {
    if (t < 0.5f) {
        return easeInBounce(t * 2.0f) * 0.5f;
    }
    return easeOutBounce(t * 2.0f - 1.0f) * 0.5f + 0.5f;
}

// Utility function to apply easing
float applyEasing(EasingType type, float t) {
    switch (type) {
        case EasingType::Linear:
            return linear(t);
            
        case EasingType::EaseInQuad:
            return easeInQuad(t);
        case EasingType::EaseOutQuad:
            return easeOutQuad(t);
        case EasingType::EaseInOutQuad:
            return easeInOutQuad(t);
            
        case EasingType::EaseInCubic:
            return easeInCubic(t);
        case EasingType::EaseOutCubic:
            return easeOutCubic(t);
        case EasingType::EaseInOutCubic:
            return easeInOutCubic(t);
            
        case EasingType::EaseInQuart:
            return easeInQuart(t);
        case EasingType::EaseOutQuart:
            return easeOutQuart(t);
        case EasingType::EaseInOutQuart:
            return easeInOutQuart(t);
            
        case EasingType::EaseInQuint:
            return easeInQuint(t);
        case EasingType::EaseOutQuint:
            return easeOutQuint(t);
        case EasingType::EaseInOutQuint:
            return easeInOutQuint(t);
            
        case EasingType::EaseInSine:
            return easeInSine(t);
        case EasingType::EaseOutSine:
            return easeOutSine(t);
        case EasingType::EaseInOutSine:
            return easeInOutSine(t);
            
        case EasingType::EaseInExpo:
            return easeInExpo(t);
        case EasingType::EaseOutExpo:
            return easeOutExpo(t);
        case EasingType::EaseInOutExpo:
            return easeInOutExpo(t);
            
        case EasingType::EaseInCirc:
            return easeInCirc(t);
        case EasingType::EaseOutCirc:
            return easeOutCirc(t);
        case EasingType::EaseInOutCirc:
            return easeInOutCirc(t);
            
        case EasingType::EaseInBack:
            return easeInBack(t);
        case EasingType::EaseOutBack:
            return easeOutBack(t);
        case EasingType::EaseInOutBack:
            return easeInOutBack(t);
            
        case EasingType::EaseInElastic:
            return easeInElastic(t);
        case EasingType::EaseOutElastic:
            return easeOutElastic(t);
        case EasingType::EaseInOutElastic:
            return easeInOutElastic(t);
            
        case EasingType::EaseInBounce:
            return easeInBounce(t);
        case EasingType::EaseOutBounce:
            return easeOutBounce(t);
        case EasingType::EaseInOutBounce:
            return easeInOutBounce(t);
            
        default:
            return linear(t);
    }
}

} // namespace Animation
} // namespace RedFiles