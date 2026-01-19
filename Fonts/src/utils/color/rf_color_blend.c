#include "rf_color_blend.h"
#include "rf_logger.h"
#include <math.h>

// Blend mode implementations
RF_Color RF_ColorBlend_Normal(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_AlphaBlend(src, dst);
}

RF_Color RF_ColorBlend_Multiply(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(src->r * dst->r, src->g * dst->g, src->b * dst->b, src->a);
}

RF_Color RF_ColorBlend_Screen(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(1.0f - (1.0f - src->r) * (1.0f - dst->r),
                          1.0f - (1.0f - src->g) * (1.0f - dst->g),
                          1.0f - (1.0f - src->b) * (1.0f - dst->b),
                          src->a);
}

RF_Color RF_ColorBlend_Overlay(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    if (dst->r < 0.5f) {
        result.r = 2.0f * src->r * dst->r;
    } else {
        result.r = 1.0f - 2.0f * (1.0f - src->r) * (1.0f - dst->r);
    }
    
    if (dst->g < 0.5f) {
        result.g = 2.0f * src->g * dst->g;
    } else {
        result.g = 1.0f - 2.0f * (1.0f - src->g) * (1.0f - dst->g);
    }
    
    if (dst->b < 0.5f) {
        result.b = 2.0f * src->b * dst->b;
    } else {
        result.b = 1.0f - 2.0f * (1.0f - src->b) * (1.0f - dst->b);
    }
    
    result.a = src->a;
    return result;
}

RF_Color RF_ColorBlend_SoftLight(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    if (dst->r < 0.5f) {
        result.r = 2.0f * src->r * dst->r + src->r * src->r * (1.0f - 2.0f * dst->r);
    } else {
        result.r = sqrtf(src->r) * (2.0f * dst->r - 1.0f) + 2.0f * src->r * (1.0f - dst->r);
    }
    
    if (dst->g < 0.5f) {
        result.g = 2.0f * src->g * dst->g + src->g * src->g * (1.0f - 2.0f * dst->g);
    } else {
        result.g = sqrtf(src->g) * (2.0f * dst->g - 1.0f) + 2.0f * src->g * (1.0f - dst->g);
    }
    
    if (dst->b < 0.5f) {
        result.b = 2.0f * src->b * dst->b + src->b * src->b * (1.0f - 2.0f * dst->b);
    } else {
        result.b = sqrtf(src->b) * (2.0f * dst->b - 1.0f) + 2.0f * src->b * (1.0f - dst->b);
    }
    
    result.a = src->a;
    return result;
}

RF_Color RF_ColorBlend_HardLight(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    if (src->r < 0.5f) {
        result.r = 2.0f * src->r * dst->r;
    } else {
        result.r = 1.0f - 2.0f * (1.0f - src->r) * (1.0f - dst->r);
    }
    
    if (src->g < 0.5f) {
        result.g = 2.0f * src->g * dst->g;
    } else {
        result.g = 1.0f - 2.0f * (1.0f - src->g) * (1.0f - dst->g);
    }
    
    if (src->b < 0.5f) {
        result.b = 2.0f * src->b * dst->b;
    } else {
        result.b = 1.0f - 2.0f * (1.0f - src->b) * (1.0f - dst->b);
    }
    
    result.a = src->a;
    return result;
}

RF_Color RF_ColorBlend_ColorDodge(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float r = dst->r < 1.0f ? RF_Clampf(src->r / (1.0f - dst->r), 0.0f, 1.0f) : 1.0f;
    float g = dst->g < 1.0f ? RF_Clampf(src->g / (1.0f - dst->g), 0.0f, 1.0f) : 1.0f;
    float b = dst->b < 1.0f ? RF_Clampf(src->b / (1.0f - dst->b), 0.0f, 1.0f) : 1.0f;
    
    return RF_Color_Create(r, g, b, src->a);
}

RF_Color RF_ColorBlend_ColorBurn(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float r = dst->r > 0.0f ? RF_Clampf(1.0f - (1.0f - src->r) / dst->r, 0.0f, 1.0f) : 0.0f;
    float g = dst->g > 0.0f ? RF_Clampf(1.0f - (1.0f - src->g) / dst->g, 0.0f, 1.0f) : 0.0f;
    float b = dst->b > 0.0f ? RF_Clampf(1.0f - (1.0f - src->b) / dst->b, 0.0f, 1.0f) : 0.0f;
    
    return RF_Color_Create(r, g, b, src->a);
}

RF_Color RF_ColorBlend_Darken(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(RF_Minf(src->r, dst->r),
                          RF_Minf(src->g, dst->g),
                          RF_Minf(src->b, dst->b),
                          src->a);
}

RF_Color RF_ColorBlend_Lighten(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(RF_Maxf(src->r, dst->r),
                          RF_Maxf(src->g, dst->g),
                          RF_Maxf(src->b, dst->b),
                          src->a);
}

RF_Color RF_ColorBlend_Difference(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(fabsf(src->r - dst->r),
                          fabsf(src->g - dst->g),
                          fabsf(src->b - dst->b),
                          src->a);
}

RF_Color RF_ColorBlend_Exclusion(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(src->r + dst->r - 2.0f * src->r * dst->r,
                          src->g + dst->g - 2.0f * src->g * dst->g,
                          src->b + dst->b - 2.0f * src->b * dst->b,
                          src->a);
}

RF_Color RF_ColorBlend_Hue(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float src_h, src_s, src_l;
    float dst_h, dst_s, dst_l;
    
    RF_Color_ToHSL(src, &src_h, &src_s, &src_l, NULL);
    RF_Color_ToHSL(dst, &dst_h, &dst_s, &dst_l, NULL);
    
    return RF_Color_CreateFromHSL(src_h, dst_s, dst_l, src->a);
}

RF_Color RF_ColorBlend_Saturation(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float src_h, src_s, src_l;
    float dst_h, dst_s, dst_l;
    
    RF_Color_ToHSL(src, &src_h, &src_s, &src_l, NULL);
    RF_Color_ToHSL(dst, &dst_h, &dst_s, &dst_l, NULL);
    
    return RF_Color_CreateFromHSL(dst_h, src_s, dst_l, src->a);
}

RF_Color RF_ColorBlend_Color(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float src_h, src_s, src_l;
    float dst_h, dst_s, dst_l;
    
    RF_Color_ToHSL(src, &src_h, &src_s, &src_l, NULL);
    RF_Color_ToHSL(dst, &dst_h, &dst_s, &dst_l, NULL);
    
    return RF_Color_CreateFromHSL(src_h, src_s, dst_l, src->a);
}

RF_Color RF_ColorBlend_Luminosity(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float src_h, src_s, src_l;
    float dst_h, dst_s, dst_l;
    
    RF_Color_ToHSL(src, &src_h, &src_s, &src_l, NULL);
    RF_Color_ToHSL(dst, &dst_h, &dst_s, &dst_l, NULL);
    
    return RF_Color_CreateFromHSL(dst_h, dst_s, src_l, src->a);
}

RF_Color RF_ColorBlend_Add(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(RF_Clampf(src->r + dst->r, 0.0f, 1.0f),
                          RF_Clampf(src->g + dst->g, 0.0f, 1.0f),
                          RF_Clampf(src->b + dst->b, 0.0f, 1.0f),
                          src->a);
}

RF_Color RF_ColorBlend_Subtract(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(RF_Clampf(src->r - dst->r, 0.0f, 1.0f),
                          RF_Clampf(src->g - dst->g, 0.0f, 1.0f),
                          RF_Clampf(src->b - dst->b, 0.0f, 1.0f),
                          src->a);
}

RF_Color RF_ColorBlend_Reflect(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    float r = src->r == 1.0f ? 1.0f : RF_Clampf(dst->r * dst->r / (1.0f - src->r), 0.0f, 1.0f);
    float g = src->g == 1.0f ? 1.0f : RF_Clampf(dst->g * dst->g / (1.0f - src->g), 0.0f, 1.0f);
    float b = src->b == 1.0f ? 1.0f : RF_Clampf(dst->b * dst->b / (1.0f - src->b), 0.0f, 1.0f);
    
    return RF_Color_Create(r, g, b, src->a);
}

RF_Color RF_ColorBlend_Glow(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    result.r = src->r == 0.0f ? 0.0f : RF_Clampf(1.0f - (1.0f - dst->r) / src->r, 0.0f, 1.0f);
    result.g = src->g == 0.0f ? 0.0f : RF_Clampf(1.0f - (1.0f - dst->g) / src->g, 0.0f, 1.0f);
    result.b = src->b == 0.0f ? 0.0f : RF_Clampf(1.0f - (1.0f - dst->b) / src->b, 0.0f, 1.0f);
    result.a = src->a;
    
    return result;
}

RF_Color RF_ColorBlend_PhotoshopOverlay(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    // Photoshop's overlay is different from standard overlay
    if (dst->r < 0.5f) {
        result.r = 2.0f * src->r * dst->r;
    } else {
        result.r = 1.0f - 2.0f * (1.0f - src->r) * (1.0f - dst->r);
    }
    
    if (dst->g < 0.5f) {
        result.g = 2.0f * src->g * dst->g;
    } else {
        result.g = 1.0f - 2.0f * (1.0f - src->g) * (1.0f - dst->g);
    }
    
    if (dst->b < 0.5f) {
        result.b = 2.0f * src->b * dst->b;
    } else {
        result.b = 1.0f - 2.0f * (1.0f - src->b) * (1.0f - dst->b);
    }
    
    result.a = src->a;
    return result;
}

RF_Color RF_ColorBlend_LinearDodge(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(RF_Clampf(src->r + dst->r, 0.0f, 1.0f),
                          RF_Clampf(src->g + dst->g, 0.0f, 1.0f),
                          RF_Clampf(src->b + dst->b, 0.0f, 1.0f),
                          src->a);
}

RF_Color RF_ColorBlend_LinearBurn(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    return RF_Color_Create(RF_Clampf(src->r + dst->r - 1.0f, 0.0f, 1.0f),
                          RF_Clampf(src->g + dst->g - 1.0f, 0.0f, 1.0f),
                          RF_Clampf(src->b + dst->b - 1.0f, 0.0f, 1.0f),
                          src->a);
}

RF_Color RF_ColorBlend_PinLight(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    result.r = dst->r < 0.5f ? RF_Minf(2.0f * src->r, dst->r) : RF_Maxf(2.0f * src->r - 1.0f, dst->r);
    result.g = dst->g < 0.5f ? RF_Minf(2.0f * src->g, dst->g) : RF_Maxf(2.0f * src->g - 1.0f, dst->g);
    result.b = dst->b < 0.5f ? RF_Minf(2.0f * src->b, dst->b) : RF_Maxf(2.0f * src->b - 1.0f, dst->b);
    result.a = src->a;
    
    return result;
}

RF_Color RF_ColorBlend_VividLight(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    if (dst->r < 0.5f) {
        result.r = dst->r == 0.0f ? 0.0f : RF_Clampf(1.0f - (1.0f - src->r) / (2.0f * dst->r), 0.0f, 1.0f);
    } else {
        result.r = dst->r == 1.0f ? 1.0f : RF_Clampf(src->r / (2.0f * (1.0f - dst->r)), 0.0f, 1.0f);
    }
    
    if (dst->g < 0.5f) {
        result.g = dst->g == 0.0f ? 0.0f : RF_Clampf(1.0f - (1.0f - src->g) / (2.0f * dst->g), 0.0f, 1.0f);
    } else {
        result.g = dst->g == 1.0f ? 1.0f : RF_Clampf(src->g / (2.0f * (1.0f - dst->g)), 0.0f, 1.0f);
    }
    
    if (dst->b < 0.5f) {
        result.b = dst->b == 0.0f ? 0.0f : RF_Clampf(1.0f - (1.0f - src->b) / (2.0f * dst->b), 0.0f, 1.0f);
    } else {
        result.b = dst->b == 1.0f ? 1.0f : RF_Clampf(src->b / (2.0f * (1.0f - dst->b)), 0.0f, 1.0f);
    }
    
    result.a = src->a;
    return result;
}

// Advanced blend functions
RF_Color RF_ColorBlend_LinearInterpolation(const RF_Color* src, const RF_Color* dst, float t)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    t = RF_Clampf(t, 0.0f, 1.0f);
    return RF_Color_Create(src->r + (dst->r - src->r) * t,
                          src->g + (dst->g - src->g) * t,
                          src->b + (dst->b - src->b) * t,
                          src->a + (dst->a - src->a) * t);
}

RF_Color RF_ColorBlend_CosineInterpolation(const RF_Color* src, const RF_Color* dst, float t)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    t = RF_Clampf(t, 0.0f, 1.0f);
    float cos_t = (1.0f - cosf(t * RF_PI)) * 0.5f;
    
    return RF_Color_Create(src->r + (dst->r - src->r) * cos_t,
                          src->g + (dst->g - src->g) * cos_t,
                          src->b + (dst->b - src->b) * cos_t,
                          src->a + (dst->a - src->a) * cos_t);
}

RF_Color RF_ColorBlend_SmoothStep(const RF_Color* src, const RF_Color* dst, float t)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    t = RF_Clampf(t, 0.0f, 1.0f);
    float smooth_t = t * t * (3.0f - 2.0f * t);
    
    return RF_Color_Create(src->r + (dst->r - src->r) * smooth_t,
                          src->g + (dst->g - src->g) * smooth_t,
                          src->b + (dst->b - src->b) * smooth_t,
                          src->a + (dst->a - src->a) * smooth_t);
}

RF_Color RF_ColorBlend_WeightedAverage(const RF_Color* colors, const float* weights, uint32_t count)
{
    if (!colors || !weights || count == 0)
        return RF_COLOR_BLACK;
    
    float total_r = 0.0f, total_g = 0.0f, total_b = 0.0f, total_a = 0.0f;
    float total_weight = 0.0f;
    
    for (uint32_t i = 0; i < count; ++i)
    {
        float w = RF_Maxf(weights[i], 0.0f);
        total_r += colors[i].r * w;
        total_g += colors[i].g * w;
        total_b += colors[i].b * w;
        total_a += colors[i].a * w;
        total_weight += w;
    }
    
    if (total_weight > 0.0f)
    {
        total_r /= total_weight;
        total_g /= total_weight;
        total_b /= total_weight;
        total_a /= total_weight;
    }
    
    return RF_Color_Create(total_r, total_g, total_b, total_a);
}

RF_Color RF_ColorBlend_MultiLayer(const RF_Color* base, const RF_Color* layers, 
                                   const RF_BlendMode* modes, uint32_t count)
{
    if (!base || !layers || !modes || count == 0)
        return base ? *base : RF_COLOR_BLACK;
    
    RF_Color result = *base;
    
    for (uint32_t i = 0; i < count; ++i)
    {
        result = RF_ColorBlend_ApplyMode(&layers[i], &result, modes[i]);
    }
    
    return result;
}

RF_Color RF_ColorBlend_ApplyMode(const RF_Color* src, const RF_Color* dst, RF_BlendMode mode)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    switch (mode)
    {
        case RF_BLEND_NORMAL: return RF_ColorBlend_Normal(src, dst);
        case RF_BLEND_MULTIPLY: return RF_ColorBlend_Multiply(src, dst);
        case RF_BLEND_SCREEN: return RF_ColorBlend_Screen(src, dst);
        case RF_BLEND_OVERLAY: return RF_ColorBlend_Overlay(src, dst);
        case RF_BLEND_SOFT_LIGHT: return RF_ColorBlend_SoftLight(src, dst);
        case RF_BLEND_HARD_LIGHT: return RF_ColorBlend_HardLight(src, dst);
        case RF_BLEND_COLOR_DODGE: return RF_ColorBlend_ColorDodge(src, dst);
        case RF_BLEND_COLOR_BURN: return RF_ColorBlend_ColorBurn(src, dst);
        case RF_BLEND_DARKEN: return RF_ColorBlend_Darken(src, dst);
        case RF_BLEND_LIGHTEN: return RF_ColorBlend_Lighten(src, dst);
        case RF_BLEND_DIFFERENCE: return RF_ColorBlend_Difference(src, dst);
        case RF_BLEND_EXCLUSION: return RF_ColorBlend_Exclusion(src, dst);
        case RF_BLEND_HUE: return RF_ColorBlend_Hue(src, dst);
        case RF_BLEND_SATURATION: return RF_ColorBlend_Saturation(src, dst);
        case RF_BLEND_COLOR: return RF_ColorBlend_Color(src, dst);
        case RF_BLEND_LUMINOSITY: return RF_ColorBlend_Luminosity(src, dst);
        case RF_BLEND_ADD: return RF_ColorBlend_Add(src, dst);
        case RF_BLEND_SUBTRACT: return RF_ColorBlend_Subtract(src, dst);
        case RF_BLEND_REFLECT: return RF_ColorBlend_Reflect(src, dst);
        case RF_BLEND_GLOW: return RF_ColorBlend_Glow(src, dst);
        case RF_BLEND_PHOTOSHOP_OVERLAY: return RF_ColorBlend_PhotoshopOverlay(src, dst);
        case RF_BLEND_LINEAR_DODGE: return RF_ColorBlend_LinearDodge(src, dst);
        case RF_BLEND_LINEAR_BURN: return RF_ColorBlend_LinearBurn(src, dst);
        case RF_BLEND_PIN_LIGHT: return RF_ColorBlend_PinLight(src, dst);
        case RF_BLEND_VIVID_LIGHT: return RF_ColorBlend_VividLight(src, dst);
        default: return RF_ColorBlend_Normal(src, dst);
    }
}

// Utility functions
const char* RF_ColorBlend_GetModeName(RF_BlendMode mode)
{
    switch (mode)
    {
        case RF_BLEND_NORMAL: return "Normal";
        case RF_BLEND_MULTIPLY: return "Multiply";
        case RF_BLEND_SCREEN: return "Screen";
        case RF_BLEND_OVERLAY: return "Overlay";
        case RF_BLEND_SOFT_LIGHT: return "Soft Light";
        case RF_BLEND_HARD_LIGHT: return "Hard Light";
        case RF_BLEND_COLOR_DODGE: return "Color Dodge";
        case RF_BLEND_COLOR_BURN: return "Color Burn";
        case RF_BLEND_DARKEN: return "Darken";
        case RF_BLEND_LIGHTEN: return "Lighten";
        case RF_BLEND_DIFFERENCE: return "Difference";
        case RF_BLEND_EXCLUSION: return "Exclusion";
        case RF_BLEND_HUE: return "Hue";
        case RF_BLEND_SATURATION: return "Saturation";
        case RF_BLEND_COLOR: return "Color";
        case RF_BLEND_LUMINOSITY: return "Luminosity";
        case RF_BLEND_ADD: return "Add";
        case RF_BLEND_SUBTRACT: return "Subtract";
        case RF_BLEND_REFLECT: return "Reflect";
        case RF_BLEND_GLOW: return "Glow";
        case RF_BLEND_PHOTOSHOP_OVERLAY: return "Photoshop Overlay";
        case RF_BLEND_LINEAR_DODGE: return "Linear Dodge";
        case RF_BLEND_LINEAR_BURN: return "Linear Burn";
        case RF_BLEND_PIN_LIGHT: return "Pin Light";
        case RF_BLEND_VIVID_LIGHT: return "Vivid Light";
        default: return "Unknown";
    }
}

bool RF_ColorBlend_IsSeparable(RF_BlendMode mode)
{
    switch (mode)
    {
        case RF_BLEND_NORMAL:
        case RF_BLEND_MULTIPLY:
        case RF_BLEND_SCREEN:
        case RF_BLEND_OVERLAY:
        case RF_BLEND_SOFT_LIGHT:
        case RF_BLEND_HARD_LIGHT:
        case RF_BLEND_COLOR_DODGE:
        case RF_BLEND_COLOR_BURN:
        case RF_BLEND_DARKEN:
        case RF_BLEND_LIGHTEN:
        case RF_BLEND_DIFFERENCE:
        case RF_BLEND_EXCLUSION:
        case RF_BLEND_ADD:
        case RF_BLEND_SUBTRACT:
        case RF_BLEND_REFLECT:
        case RF_BLEND_GLOW:
        case RF_BLEND_PHOTOSHOP_OVERLAY:
        case RF_BLEND_LINEAR_DODGE:
        case RF_BLEND_LINEAR_BURN:
        case RF_BLEND_PIN_LIGHT:
        case RF_BLEND_VIVID_LIGHT:
            return true;
        
        case RF_BLEND_HUE:
        case RF_BLEND_SATURATION:
        case RF_BLEND_COLOR:
        case RF_BLEND_LUMINOSITY:
            return false;
        
        default:
            return true;
    }
}

bool RF_ColorBlend_IsNonSeparable(RF_BlendMode mode)
{
    return !RF_ColorBlend_IsSeparable(mode);
}

bool RF_ColorBlend_IsCommutative(RF_BlendMode mode)
{
    switch (mode)
    {
        case RF_BLEND_NORMAL:
        case RF_BLEND_MULTIPLY:
        case RF_BLEND_SCREEN:
        case RF_BLEND_DARKEN:
        case RF_BLEND_LIGHTEN:
        case RF_BLEND_DIFFERENCE:
        case RF_BLEND_EXCLUSION:
        case RF_BLEND_ADD:
            return true;
        
        case RF_BLEND_OVERLAY:
        case RF_BLEND_SOFT_LIGHT:
        case RF_BLEND_HARD_LIGHT:
        case RF_BLEND_COLOR_DODGE:
        case RF_BLEND_COLOR_BURN:
        case RF_BLEND_HUE:
        case RF_BLEND_SATURATION:
        case RF_BLEND_COLOR:
        case RF_BLEND_LUMINOSITY:
        case RF_BLEND_SUBTRACT:
        case RF_BLEND_REFLECT:
        case RF_BLEND_GLOW:
        case RF_BLEND_PHOTOSHOP_OVERLAY:
        case RF_BLEND_LINEAR_DODGE:
        case RF_BLEND_LINEAR_BURN:
        case RF_BLEND_PIN_LIGHT:
        case RF_BLEND_VIVID_LIGHT:
            return false;
        
        default:
            return false;
    }
}

bool RF_ColorBlend_IsAssociative(RF_BlendMode mode)
{
    switch (mode)
    {
        case RF_BLEND_NORMAL:
        case RF_BLEND_MULTIPLY:
        case RF_BLEND_SCREEN:
        case RF_BLEND_DARKEN:
        case RF_BLEND_LIGHTEN:
        case RF_BLEND_ADD:
            return true;
        
        case RF_BLEND_OVERLAY:
        case RF_BLEND_SOFT_LIGHT:
        case RF_BLEND_HARD_LIGHT:
        case RF_BLEND_COLOR_DODGE:
        case RF_BLEND_COLOR_BURN:
        case RF_BLEND_DIFFERENCE:
        case RF_BLEND_EXCLUSION:
        case RF_BLEND_HUE:
        case RF_BLEND_SATURATION:
        case RF_BLEND_COLOR:
        case RF_BLEND_LUMINOSITY:
        case RF_BLEND_SUBTRACT:
        case RF_BLEND_REFLECT:
        case RF_BLEND_GLOW:
        case RF_BLEND_PHOTOSHOP_OVERLAY:
        case RF_BLEND_LINEAR_DODGE:
        case RF_BLEND_LINEAR_BURN:
        case RF_BLEND_PIN_LIGHT:
        case RF_BLEND_VIVID_LIGHT:
            return false;
        
        default:
            return false;
    }
}

void RF_ColorBlend_PrintMode(RF_BlendMode mode)
{
    RF_LOG_INFO("Blend Mode: ", RF_ColorBlend_GetModeName(mode));
    RF_LOG_INFO("Separable: ", RF_ColorBlend_IsSeparable(mode) ? "Yes" : "No");
    RF_LOG_INFO("Commutative: ", RF_ColorBlend_IsCommutative(mode) ? "Yes" : "No");
    RF_LOG_INFO("Associative: ", RF_ColorBlend_IsAssociative(mode) ? "Yes" : "No");
}

// Performance optimization functions
RF_Color RF_ColorBlend_FastMultiply(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    // Fast multiply without clamping (assumes inputs are already clamped)
    return RF_Color_Create(src->r * dst->r, src->g * dst->g, src->b * dst->b, src->a);
}

RF_Color RF_ColorBlend_FastScreen(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    // Fast screen without clamping (assumes inputs are already clamped)
    return RF_Color_Create(src->r + dst->r - src->r * dst->r,
                          src->g + dst->g - src->g * dst->g,
                          src->b + dst->b - src->b * dst->b,
                          src->a);
}

RF_Color RF_ColorBlend_FastAdd(const RF_Color* src, const RF_Color* dst)
{
    if (!src || !dst)
        return RF_COLOR_BLACK;
    
    // Fast add without clamping (assumes inputs won't overflow)
    return RF_Color_Create(src->r + dst->r, src->g + dst->g, src->b + dst->b, src->a);
}

// Batch operations
void RF_ColorBlend_BatchApply(RF_Color* results, const RF_Color* src, const RF_Color* dst, 
                               RF_BlendMode mode, uint32_t count)
{
    if (!results || !src || !dst || count == 0)
        return;
    
    for (uint32_t i = 0; i < count; ++i)
    {
        results[i] = RF_ColorBlend_ApplyMode(&src[i], &dst[i], mode);
    }
}

void RF_ColorBlend_BatchLerp(RF_Color* results, const RF_Color* src, const RF_Color* dst, 
                             float t, uint32_t count)
{
    if (!results || !src || !dst || count == 0)
        return;
    
    float clamped_t = RF_Clampf(t, 0.0f, 1.0f);
    
    for (uint32_t i = 0; i < count; ++i)
    {
        results[i] = RF_Color_Create(src[i].r + (dst[i].r - src[i].r) * clamped_t,
                                    src[i].g + (dst[i].g - src[i].g) * clamped_t,
                                    src[i].b + (dst[i].b - src[i].b) * clamped_t,
                                    src[i].a + (dst[i].a - src[i].a) * clamped_t);
    }
}

// Utility functions
float RF_Clampf(float value, float min, float max)
{
    return value < min ? min : (value > max ? max : value);
}

float RF_Maxf(float a, float b)
{
    return a > b ? a : b;
}

float RF_Minf(float a, float b)
{
    return a < b ? a : b;
}

// Constants
const float RF_PI = 3.14159265358979323846f;