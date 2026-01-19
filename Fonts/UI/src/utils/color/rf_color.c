#include "rf_color.h"
#include "rf_logger.h"
#include <math.h>
#include <string.h>

// Predefined colors
const RF_Color RF_COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const RF_Color RF_COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const RF_Color RF_COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
const RF_Color RF_COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const RF_Color RF_COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
const RF_Color RF_COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
const RF_Color RF_COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
const RF_Color RF_COLOR_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
const RF_Color RF_COLOR_GRAY = {0.5f, 0.5f, 0.5f, 1.0f};
const RF_Color RF_COLOR_LIGHT_GRAY = {0.75f, 0.75f, 0.75f, 1.0f};
const RF_Color RF_COLOR_DARK_GRAY = {0.25f, 0.25f, 0.25f, 1.0f};
const RF_Color RF_COLOR_TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};

RF_Color RF_Color_Create(float r, float g, float b, float a)
{
    RF_Color color;
    color.r = RF_Clampf(r, 0.0f, 1.0f);
    color.g = RF_Clampf(g, 0.0f, 1.0f);
    color.b = RF_Clampf(b, 0.0f, 1.0f);
    color.a = RF_Clampf(a, 0.0f, 1.0f);
    return color;
}

RF_Color RF_Color_CreateRGB(float r, float g, float b)
{
    return RF_Color_Create(r, g, b, 1.0f);
}

RF_Color RF_Color_CreateRGBA(float r, float g, float b, float a)
{
    return RF_Color_Create(r, g, b, a);
}

RF_Color RF_Color_CreateFromHex(uint32_t hex)
{
    RF_Color color;
    color.r = ((hex >> 24) & 0xFF) / 255.0f;
    color.g = ((hex >> 16) & 0xFF) / 255.0f;
    color.b = ((hex >> 8) & 0xFF) / 255.0f;
    color.a = (hex & 0xFF) / 255.0f;
    return color;
}

RF_Color RF_Color_CreateFromHexRGB(uint32_t hex)
{
    RF_Color color;
    color.r = ((hex >> 16) & 0xFF) / 255.0f;
    color.g = ((hex >> 8) & 0xFF) / 255.0f;
    color.b = (hex & 0xFF) / 255.0f;
    color.a = 1.0f;
    return color;
}

RF_Color RF_Color_CreateFromHexARGB(uint32_t hex)
{
    RF_Color color;
    color.a = ((hex >> 24) & 0xFF) / 255.0f;
    color.r = ((hex >> 16) & 0xFF) / 255.0f;
    color.g = ((hex >> 8) & 0xFF) / 255.0f;
    color.b = (hex & 0xFF) / 255.0f;
    return color;
}

RF_Color RF_Color_CreateFromHSL(float h, float s, float l, float a)
{
    h = RF_Clampf(h, 0.0f, 360.0f);
    s = RF_Clampf(s, 0.0f, 1.0f);
    l = RF_Clampf(l, 0.0f, 1.0f);
    a = RF_Clampf(a, 0.0f, 1.0f);

    float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
    float h_prime = h / 60.0f;
    float x = c * (1.0f - fabsf(fmodf(h_prime, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    float r1, g1, b1;
    
    if (h_prime >= 0 && h_prime < 1) {
        r1 = c; g1 = x; b1 = 0;
    } else if (h_prime >= 1 && h_prime < 2) {
        r1 = x; g1 = c; b1 = 0;
    } else if (h_prime >= 2 && h_prime < 3) {
        r1 = 0; g1 = c; b1 = x;
    } else if (h_prime >= 3 && h_prime < 4) {
        r1 = 0; g1 = x; b1 = c;
    } else if (h_prime >= 4 && h_prime < 5) {
        r1 = x; g1 = 0; b1 = c;
    } else {
        r1 = c; g1 = 0; b1 = x;
    }

    return RF_Color_Create(r1 + m, g1 + m, b1 + m, a);
}

RF_Color RF_Color_CreateFromHSV(float h, float s, float v, float a)
{
    h = RF_Clampf(h, 0.0f, 360.0f);
    s = RF_Clampf(s, 0.0f, 1.0f);
    v = RF_Clampf(v, 0.0f, 1.0f);
    a = RF_Clampf(a, 0.0f, 1.0f);

    float c = v * s;
    float h_prime = h / 60.0f;
    float x = c * (1.0f - fabsf(fmodf(h_prime, 2.0f) - 1.0f));
    float m = v - c;

    float r1, g1, b1;
    
    if (h_prime >= 0 && h_prime < 1) {
        r1 = c; g1 = x; b1 = 0;
    } else if (h_prime >= 1 && h_prime < 2) {
        r1 = x; g1 = c; b1 = 0;
    } else if (h_prime >= 2 && h_prime < 3) {
        r1 = 0; g1 = c; b1 = x;
    } else if (h_prime >= 3 && h_prime < 4) {
        r1 = 0; g1 = x; b1 = c;
    } else if (h_prime >= 4 && h_prime < 5) {
        r1 = x; g1 = 0; b1 = c;
    } else {
        r1 = c; g1 = 0; b1 = x;
    }

    return RF_Color_Create(r1 + m, g1 + m, b1 + m, a);
}

RF_Color RF_Color_Clone(const RF_Color* color)
{
    if (!color)
        return RF_COLOR_BLACK;
    return *color;
}

void RF_Color_Copy(RF_Color* dest, const RF_Color* src)
{
    if (!dest || !src)
        return;
    *dest = *src;
}

bool RF_Color_Equals(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return false;
    return fabsf(a->r - b->r) < RF_COLOR_EPSILON &&
           fabsf(a->g - b->g) < RF_COLOR_EPSILON &&
           fabsf(a->b - b->b) < RF_COLOR_EPSILON &&
           fabsf(a->a - b->a) < RF_COLOR_EPSILON;
}

bool RF_Color_EqualsRGB(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return false;
    return fabsf(a->r - b->r) < RF_COLOR_EPSILON &&
           fabsf(a->g - b->g) < RF_COLOR_EPSILON &&
           fabsf(a->b - b->b) < RF_COLOR_EPSILON;
}

void RF_Color_Set(RF_Color* color, float r, float g, float b, float a)
{
    if (!color)
        return;
    color->r = RF_Clampf(r, 0.0f, 1.0f);
    color->g = RF_Clampf(g, 0.0f, 1.0f);
    color->b = RF_Clampf(b, 0.0f, 1.0f);
    color->a = RF_Clampf(a, 0.0f, 1.0f);
}

void RF_Color_SetRGB(RF_Color* color, float r, float g, float b)
{
    RF_Color_Set(color, r, g, b, 1.0f);
}

void RF_Color_SetRGBA(RF_Color* color, float r, float g, float b, float a)
{
    RF_Color_Set(color, r, g, b, a);
}

void RF_Color_SetHex(RF_Color* color, uint32_t hex)
{
    if (!color)
        return;
    color->r = ((hex >> 24) & 0xFF) / 255.0f;
    color->g = ((hex >> 16) & 0xFF) / 255.0f;
    color->b = ((hex >> 8) & 0xFF) / 255.0f;
    color->a = (hex & 0xFF) / 255.0f;
}

void RF_Color_SetHexRGB(RF_Color* color, uint32_t hex)
{
    if (!color)
        return;
    color->r = ((hex >> 16) & 0xFF) / 255.0f;
    color->g = ((hex >> 8) & 0xFF) / 255.0f;
    color->b = (hex & 0xFF) / 255.0f;
    color->a = 1.0f;
}

void RF_Color_SetHexARGB(RF_Color* color, uint32_t hex)
{
    if (!color)
        return;
    color->a = ((hex >> 24) & 0xFF) / 255.0f;
    color->r = ((hex >> 16) & 0xFF) / 255.0f;
    color->g = ((hex >> 8) & 0xFF) / 255.0f;
    color->b = (hex & 0xFF) / 255.0f;
}

void RF_Color_SetHSL(RF_Color* color, float h, float s, float l, float a)
{
    if (!color)
        return;
    *color = RF_Color_CreateFromHSL(h, s, l, a);
}

void RF_Color_SetHSV(RF_Color* color, float h, float s, float v, float a)
{
    if (!color)
        return;
    *color = RF_Color_CreateFromHSV(h, s, v, a);
}

uint32_t RF_Color_ToHex(const RF_Color* color)
{
    if (!color)
        return 0x000000FF;
    return ((uint32_t)(color->r * 255.0f) << 24) |
           ((uint32_t)(color->g * 255.0f) << 16) |
           ((uint32_t)(color->b * 255.0f) << 8) |
           (uint32_t)(color->a * 255.0f);
}

uint32_t RF_Color_ToHexRGB(const RF_Color* color)
{
    if (!color)
        return 0x000000;
    return ((uint32_t)(color->r * 255.0f) << 16) |
           ((uint32_t)(color->g * 255.0f) << 8) |
           (uint32_t)(color->b * 255.0f);
}

uint32_t RF_Color_ToHexARGB(const RF_Color* color)
{
    if (!color)
        return 0xFF000000;
    return ((uint32_t)(color->a * 255.0f) << 24) |
           ((uint32_t)(color->r * 255.0f) << 16) |
           ((uint32_t)(color->g * 255.0f) << 8) |
           (uint32_t)(color->b * 255.0f);
}

void RF_Color_ToHSL(const RF_Color* color, float* h, float* s, float* l, float* a)
{
    if (!color)
        return;

    float r = color->r;
    float g = color->g;
    float b = color->b;

    float max_val = RF_Maxf(RF_Maxf(r, g), b);
    float min_val = RF_Minf(RF_Minf(r, g), b);
    float delta = max_val - min_val;

    // Lightness
    float lightness = (max_val + min_val) / 2.0f;

    // Saturation
    float saturation = 0.0f;
    if (delta > 0.0f) {
        saturation = delta / (1.0f - fabsf(2.0f * lightness - 1.0f));
    }

    // Hue
    float hue = 0.0f;
    if (delta > 0.0f) {
        if (max_val == r) {
            hue = 60.0f * fmodf((g - b) / delta, 6.0f);
        } else if (max_val == g) {
            hue = 60.0f * ((b - r) / delta + 2.0f);
        } else {
            hue = 60.0f * ((r - g) / delta + 4.0f);
        }
    }

    if (h) *h = hue;
    if (s) *s = saturation;
    if (l) *l = lightness;
    if (a) *a = color->a;
}

void RF_Color_ToHSV(const RF_Color* color, float* h, float* s, float* v, float* a)
{
    if (!color)
        return;

    float r = color->r;
    float g = color->g;
    float b = color->b;

    float max_val = RF_Maxf(RF_Maxf(r, g), b);
    float min_val = RF_Minf(RF_Minf(r, g), b);
    float delta = max_val - min_val;

    // Value
    float value = max_val;

    // Saturation
    float saturation = 0.0f;
    if (max_val > 0.0f) {
        saturation = delta / max_val;
    }

    // Hue
    float hue = 0.0f;
    if (delta > 0.0f) {
        if (max_val == r) {
            hue = 60.0f * fmodf((g - b) / delta, 6.0f);
        } else if (max_val == g) {
            hue = 60.0f * ((b - r) / delta + 2.0f);
        } else {
            hue = 60.0f * ((r - g) / delta + 4.0f);
        }
    }

    if (h) *h = hue;
    if (s) *s = saturation;
    if (v) *v = value;
    if (a) *a = color->a;
}

float RF_Color_GetLuminance(const RF_Color* color)
{
    if (!color)
        return 0.0f;
    return 0.2126f * color->r + 0.7152f * color->g + 0.0722f * color->b;
}

float RF_Color_GetBrightness(const RF_Color* color)
{
    if (!color)
        return 0.0f;
    return (color->r + color->g + color->b) / 3.0f;
}

float RF_Color_GetIntensity(const RF_Color* color)
{
    if (!color)
        return 0.0f;
    return RF_Maxf(RF_Maxf(color->r, color->g), color->b);
}

bool RF_Color_IsTransparent(const RF_Color* color)
{
    if (!color)
        return true;
    return color->a < RF_COLOR_EPSILON;
}

bool RF_Color_IsOpaque(const RF_Color* color)
{
    if (!color)
        return false;
    return color->a > (1.0f - RF_COLOR_EPSILON);
}

bool RF_Color_IsBlack(const RF_Color* color)
{
    if (!color)
        return false;
    return color->r < RF_COLOR_EPSILON &&
           color->g < RF_COLOR_EPSILON &&
           color->b < RF_COLOR_EPSILON;
}

bool RF_Color_IsWhite(const RF_Color* color)
{
    if (!color)
        return false;
    return color->r > (1.0f - RF_COLOR_EPSILON) &&
           color->g > (1.0f - RF_COLOR_EPSILON) &&
           color->b > (1.0f - RF_COLOR_EPSILON);
}

bool RF_Color_IsGray(const RF_Color* color)
{
    if (!color)
        return false;
    return fabsf(color->r - color->g) < RF_COLOR_EPSILON &&
           fabsf(color->g - color->b) < RF_COLOR_EPSILON;
}

RF_Color RF_Color_Invert(const RF_Color* color)
{
    if (!color)
        return RF_COLOR_BLACK;
    return RF_Color_Create(1.0f - color->r, 1.0f - color->g, 1.0f - color->b, color->a);
}

RF_Color RF_Color_InvertRGB(const RF_Color* color)
{
    if (!color)
        return RF_COLOR_BLACK;
    return RF_Color_Create(1.0f - color->r, 1.0f - color->g, 1.0f - color->b, color->a);
}

RF_Color RF_Color_GrayScale(const RF_Color* color)
{
    if (!color)
        return RF_COLOR_BLACK;
    float gray = RF_Color_GetLuminance(color);
    return RF_Color_Create(gray, gray, gray, color->a);
}

RF_Color RF_Color_Sepia(const RF_Color* color)
{
    if (!color)
        return RF_COLOR_BLACK;
    
    float r = color->r;
    float g = color->g;
    float b = color->b;
    
    float tr = 0.393f * r + 0.769f * g + 0.189f * b;
    float tg = 0.349f * r + 0.686f * g + 0.168f * b;
    float tb = 0.272f * r + 0.534f * g + 0.131f * b;
    
    return RF_Color_Create(RF_Clampf(tr, 0.0f, 1.0f),
                          RF_Clampf(tg, 0.0f, 1.0f),
                          RF_Clampf(tb, 0.0f, 1.0f),
                          color->a);
}

RF_Color RF_Color_Brightness(const RF_Color* color, float factor)
{
    if (!color)
        return RF_COLOR_BLACK;
    return RF_Color_Create(RF_Clampf(color->r * factor, 0.0f, 1.0f),
                          RF_Clampf(color->g * factor, 0.0f, 1.0f),
                          RF_Clampf(color->b * factor, 0.0f, 1.0f),
                          color->a);
}

RF_Color RF_Color_Contrast(const RF_Color* color, float factor)
{
    if (!color)
        return RF_COLOR_BLACK;
    
    float adjusted = (color->r - 0.5f) * factor + 0.5f;
    float r = RF_Clampf(adjusted, 0.0f, 1.0f);
    
    adjusted = (color->g - 0.5f) * factor + 0.5f;
    float g = RF_Clampf(adjusted, 0.0f, 1.0f);
    
    adjusted = (color->b - 0.5f) * factor + 0.5f;
    float b = RF_Clampf(adjusted, 0.0f, 1.0f);
    
    return RF_Color_Create(r, g, b, color->a);
}

RF_Color RF_Color_Saturate(const RF_Color* color, float factor)
{
    if (!color)
        return RF_COLOR_BLACK;
    
    float h, s, l, a;
    RF_Color_ToHSL(color, &h, &s, &l, &a);
    s = RF_Clampf(s * factor, 0.0f, 1.0f);
    return RF_Color_CreateFromHSL(h, s, l, a);
}

RF_Color RF_Color_Desaturate(const RF_Color* color, float factor)
{
    if (!color)
        return RF_COLOR_BLACK;
    
    float gray = RF_Color_GetLuminance(color);
    float r = color->r + (gray - color->r) * factor;
    float g = color->g + (gray - color->g) * factor;
    float b = color->b + (gray - color->b) * factor;
    
    return RF_Color_Create(r, g, b, color->a);
}

RF_Color RF_Color_HueShift(const RF_Color* color, float degrees)
{
    if (!color)
        return RF_COLOR_BLACK;
    
    float h, s, l, a;
    RF_Color_ToHSL(color, &h, &s, &l, &a);
    h = fmodf(h + degrees, 360.0f);
    if (h < 0) h += 360.0f;
    return RF_Color_CreateFromHSL(h, s, l, a);
}

RF_Color RF_Color_AlphaBlend(const RF_Color* foreground, const RF_Color* background)
{
    if (!foreground || !background)
        return RF_COLOR_BLACK;
    
    float a = foreground->a + background->a * (1.0f - foreground->a);
    if (a < RF_COLOR_EPSILON)
        return RF_COLOR_TRANSPARENT;
    
    float r = (foreground->r * foreground->a + background->r * background->a * (1.0f - foreground->a)) / a;
    float g = (foreground->g * foreground->a + background->g * background->a * (1.0f - foreground->a)) / a;
    float b = (foreground->b * foreground->a + background->b * background->a * (1.0f - foreground->a)) / a;
    
    return RF_Color_Create(r, g, b, a);
}

RF_Color RF_Color_Multiply(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    return RF_Color_Create(a->r * b->r, a->g * b->g, a->b * b->b, a->a * b->a);
}

RF_Color RF_Color_Screen(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    return RF_Color_Create(1.0f - (1.0f - a->r) * (1.0f - b->r),
                          1.0f - (1.0f - a->g) * (1.0f - b->g),
                          1.0f - (1.0f - a->b) * (1.0f - b->b),
                          a->a);
}

RF_Color RF_Color_Overlay(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    if (a->r < 0.5f) {
        result.r = 2.0f * a->r * b->r;
    } else {
        result.r = 1.0f - 2.0f * (1.0f - a->r) * (1.0f - b->r);
    }
    
    if (a->g < 0.5f) {
        result.g = 2.0f * a->g * b->g;
    } else {
        result.g = 1.0f - 2.0f * (1.0f - a->g) * (1.0f - b->g);
    }
    
    if (a->b < 0.5f) {
        result.b = 2.0f * a->b * b->b;
    } else {
        result.b = 1.0f - 2.0f * (1.0f - a->b) * (1.0f - b->b);
    }
    
    result.a = a->a;
    return result;
}

RF_Color RF_Color_SoftLight(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    if (b->r < 0.5f) {
        result.r = 2.0f * a->r * b->r + a->r * a->r * (1.0f - 2.0f * b->r);
    } else {
        result.r = sqrtf(a->r) * (2.0f * b->r - 1.0f) + 2.0f * a->r * (1.0f - b->r);
    }
    
    if (b->g < 0.5f) {
        result.g = 2.0f * a->g * b->g + a->g * a->g * (1.0f - 2.0f * b->g);
    } else {
        result.g = sqrtf(a->g) * (2.0f * b->g - 1.0f) + 2.0f * a->g * (1.0f - b->g);
    }
    
    if (b->b < 0.5f) {
        result.b = 2.0f * a->b * b->b + a->b * a->b * (1.0f - 2.0f * b->b);
    } else {
        result.b = sqrtf(a->b) * (2.0f * b->b - 1.0f) + 2.0f * a->b * (1.0f - b->b);
    }
    
    result.a = a->a;
    return result;
}

RF_Color RF_Color_HardLight(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    
    RF_Color result;
    
    if (b->r < 0.5f) {
        result.r = 2.0f * a->r * b->r;
    } else {
        result.r = 1.0f - 2.0f * (1.0f - a->r) * (1.0f - b->r);
    }
    
    if (b->g < 0.5f) {
        result.g = 2.0f * a->g * b->g;
    } else {
        result.g = 1.0f - 2.0f * (1.0f - a->g) * (1.0f - b->g);
    }
    
    if (b->b < 0.5f) {
        result.b = 2.0f * a->b * b->b;
    } else {
        result.b = 1.0f - 2.0f * (1.0f - a->b) * (1.0f - b->b);
    }
    
    result.a = a->a;
    return result;
}

RF_Color RF_Color_Dodge(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    
    float r = b->r < 1.0f ? RF_Clampf(a->r / (1.0f - b->r), 0.0f, 1.0f) : 1.0f;
    float g = b->g < 1.0f ? RF_Clampf(a->g / (1.0f - b->g), 0.0f, 1.0f) : 1.0f;
    float b_result = b->b < 1.0f ? RF_Clampf(a->b / (1.0f - b->b), 0.0f, 1.0f) : 1.0f;
    
    return RF_Color_Create(r, g, b_result, a->a);
}

RF_Color RF_Color_Burn(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    
    float r = b->r > 0.0f ? RF_Clampf(1.0f - (1.0f - a->r) / b->r, 0.0f, 1.0f) : 0.0f;
    float g = b->g > 0.0f ? RF_Clampf(1.0f - (1.0f - a->g) / b->g, 0.0f, 1.0f) : 0.0f;
    float b_result = b->b > 0.0f ? RF_Clampf(1.0f - (1.0f - a->b) / b->b, 0.0f, 1.0f) : 0.0f;
    
    return RF_Color_Create(r, g, b_result, a->a);
}

RF_Color RF_Color_Lerp(const RF_Color* a, const RF_Color* b, float t)
{
    if (!a || !b)
        return RF_COLOR_BLACK;
    
    t = RF_Clampf(t, 0.0f, 1.0f);
    return RF_Color_Create(a->r + (b->r - a->r) * t,
                          a->g + (b->g - a->g) * t,
                          a->b + (b->b - a->b) * t,
                          a->a + (b->a - a->a) * t);
}

float RF_Color_Distance(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return 0.0f;
    
    float dr = a->r - b->r;
    float dg = a->g - b->g;
    float db = a->b - b->b;
    float da = a->a - b->a;
    
    return sqrtf(dr * dr + dg * dg + db * db + da * da);
}

float RF_Color_DistanceRGB(const RF_Color* a, const RF_Color* b)
{
    if (!a || !b)
        return 0.0f;
    
    float dr = a->r - b->r;
    float dg = a->g - b->g;
    float db = a->b - b->b;
    
    return sqrtf(dr * dr + dg * dg + db * db);
}

void RF_Color_Print(const RF_Color* color)
{
    if (!color)
    {
        RF_LOG_INFO("Color: NULL");
        return;
    }
    
    RF_LOG_INFO("Color: R=", color->r, " G=", color->g, " B=", color->b, " A=", color->a);
    RF_LOG_INFO("Hex: 0x", RF_Color_ToHex(color));
    RF_LOG_INFO("RGB: ", RF_Color_ToHexRGB(color));
    RF_LOG_INFO("ARGB: ", RF_Color_ToHexARGB(color));
    
    float h, s, l;
    RF_Color_ToHSL(color, &h, &s, &l, NULL);
    RF_LOG_INFO("HSL: H=", h, "° S=", s, " L=", l);
    
    RF_LOG_INFO("Luminance: ", RF_Color_GetLuminance(color));
    RF_LOG_INFO("Brightness: ", RF_Color_GetBrightness(color));
}

void RF_Color_PrintHex(const RF_Color* color)
{
    if (!color)
    {
        RF_LOG_INFO("Color: NULL");
        return;
    }
    
    RF_LOG_INFO("0x", RF_Color_ToHex(color));
}

void RF_Color_PrintRGB(const RF_Color* color)
{
    if (!color)
    {
        RF_LOG_INFO("Color: NULL");
        return;
    }
    
    RF_LOG_INFO("rgb(", (int)(color->r * 255), ", ", (int)(color->g * 255), ", ", (int)(color->b * 255), ")");
}

void RF_Color_PrintRGBA(const RF_Color* color)
{
    if (!color)
    {
        RF_LOG_INFO("Color: NULL");
        return;
    }
    
    RF_LOG_INFO("rgba(", (int)(color->r * 255), ", ", (int)(color->g * 255), ", ", (int)(color->b * 255), ", ", color->a, ")");
}

void RF_Color_PrintHSL(const RF_Color* color)
{
    if (!color)
    {
        RF_LOG_INFO("Color: NULL");
        return;
    }
    
    float h, s, l;
    RF_Color_ToHSL(color, &h, &s, &l, NULL);
    RF_LOG_INFO("hsl(", h, ", ", s * 100, "%, ", l * 100, "%)");
}

void RF_Color_PrintHSV(const RF_Color* color)
{
    if (!color)
    {
        RF_LOG_INFO("Color: NULL");
        return;
    }
    
    float h, s, v;
    RF_Color_ToHSV(color, &h, &s, &v, NULL);
    RF_LOG_INFO("hsv(", h, ", ", s * 100, "%, ", v * 100, "%)");
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