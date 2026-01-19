#include "rf_color_gradient.h"
#include "rf_logger.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Gradient stop structure
typedef struct RF_GradientStop
{
    RF_Color color;
    float position;
    struct RF_GradientStop* next;
} RF_GradientStop;

// Gradient structure
struct RF_Gradient
{
    RF_GradientStop* stops;
    uint32_t stopCount;
    RF_GradientType type;
    RF_GradientSpread spread;
    RF_ColorInterpolation interpolation;
    bool isDirty;
    RF_Color* cachedColors;
    uint32_t cacheSize;
};

// Utility functions
static RF_GradientStop* CreateGradientStop(const RF_Color* color, float position);
static void DestroyGradientStop(RF_GradientStop* stop);
static void SortGradientStops(RF_Gradient* gradient);
static RF_Color InterpolateColor(const RF_Color* a, const RF_Color* b, float t, RF_ColorInterpolation interpolation);
static RF_Color EvaluateGradientAt(const RF_Gradient* gradient, float t);

RF_Gradient* RF_Gradient_Create(RF_GradientType type)
{
    RF_Gradient* gradient = (RF_Gradient*)malloc(sizeof(RF_Gradient));
    if (!gradient)
    {
        RF_LOG_ERROR("RF_Gradient: Failed to allocate gradient structure");
        return NULL;
    }

    gradient->stops = NULL;
    gradient->stopCount = 0;
    gradient->type = type;
    gradient->spread = RF_GRADIENT_SPREAD_PAD;
    gradient->interpolation = RF_COLOR_INTERPOLATION_LINEAR;
    gradient->isDirty = true;
    gradient->cachedColors = NULL;
    gradient->cacheSize = 0;

    RF_LOG_INFO("RF_Gradient: Created gradient with type ", (int)type);
    return gradient;
}

void RF_Gradient_Destroy(RF_Gradient* gradient)
{
    if (!gradient)
    {
        RF_LOG_WARNING("RF_Gradient: Cannot destroy null gradient");
        return;
    }

    // Destroy all stops
    RF_GradientStop* current = gradient->stops;
    while (current)
    {
        RF_GradientStop* next = current->next;
        DestroyGradientStop(current);
        current = next;
    }

    // Free cache
    if (gradient->cachedColors)
    {
        free(gradient->cachedColors);
    }

    free(gradient);
    RF_LOG_INFO("RF_Gradient: Destroyed gradient");
}

bool RF_Gradient_AddStop(RF_Gradient* gradient, const RF_Color* color, float position)
{
    if (!gradient || !color)
    {
        RF_LOG_ERROR("RF_Gradient: Invalid parameters for adding stop");
        return false;
    }

    // Clamp position
    position = RF_Clampf(position, 0.0f, 1.0f);

    // Create new stop
    RF_GradientStop* stop = CreateGradientStop(color, position);
    if (!stop)
    {
        RF_LOG_ERROR("RF_Gradient: Failed to create gradient stop");
        return false;
    }

    // Insert stop in sorted order
    RF_GradientStop** current = &gradient->stops;
    while (*current && (*current)->position < position)
    {
        current = &(*current)->next;
    }

    stop->next = *current;
    *current = stop;
    gradient->stopCount++;
    gradient->isDirty = true;

    RF_LOG_DEBUG("RF_Gradient: Added stop at position ", position);
    return true;
}

bool RFradient_RemoveStop(RF_Gradient* gradient, uint32_t index)
{
    if (!gradient || index >= gradient->stopCount)
    {
        RF_LOG_ERROR("RF_Gradient: Invalid parameters for removing stop");
        return false;
    }

    // Find and remove stop
    if (index == 0)
    {
        RF_GradientStop* stop = gradient->stops;
        gradient->stops = stop->next;
        DestroyGradientStop(stop);
    }
    else
    {
        RF_GradientStop* current = gradient->stops;
        for (uint32_t i = 0; i < index - 1; ++i)
        {
            current = current->next;
            if (!current)
            {
                RF_LOG_ERROR("RF_Gradient: Invalid stop index");
                return false;
            }
        }
        RF_GradientStop* stop = current->next;
        current->next = stop->next;
        DestroyGradientStop(stop);
    }

    gradient->stopCount--;
    gradient->isDirty = true;

    RF_LOG_DEBUG("RF_Gradient: Removed stop at index ", index);
    return true;
}

bool RF_Gradient_ClearStops(RF_Gradient* gradient)
{
    if (!gradient)
    {
        RF_LOG_ERROR("RF_Gradient: Cannot clear stops of null gradient");
        return false;
    }

    // Destroy all stops
    RF_GradientStop* current = gradient->stops;
    while (current)
    {
        RF_GradientStop* next = current->next;
        DestroyGradientStop(current);
        current = next;
    }

    gradient->stops = NULL;
    gradient->stopCount = 0;
    gradient->isDirty = true;

    RF_LOG_DEBUG("RF_Gradient: Cleared all stops");
    return true;
}

void RF_Gradient_SetType(RF_Gradient* gradient, RF_GradientType type)
{
    if (!gradient)
        return;

    gradient->type = type;
    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Set type to ", (int)type);
}

void RF_Gradient_SetSpread(RF_Gradient* gradient, RF_GradientSpread spread)
{
    if (!gradient)
        return;

    gradient->spread = spread;
    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Set spread to ", (int)spread);
}

void RF_Gradient_SetInterpolation(RF_Gradient* gradient, RF_ColorInterpolation interpolation)
{
    if (!gradient)
        return;

    gradient->interpolation = interpolation;
    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Set interpolation to ", (int)interpolation);
}

RF_GradientType RF_Gradient_GetType(const RF_Gradient* gradient)
{
    if (!gradient)
        return RF_GRADIENT_LINEAR;
    return gradient->type;
}

RF_GradientSpread RF_Gradient_GetSpread(const RF_Gradient* gradient)
{
    if (!gradient)
        return RF_GRADIENT_SPREAD_PAD;
    return gradient->spread;
}

RF_ColorInterpolation RF_Gradient_GetInterpolation(const RF_Gradient* gradient)
{
    if (!gradient)
        return RF_COLOR_INTERPOLATION_LINEAR;
    return gradient->interpolation;
}

uint32_t RF_Gradient_GetStopCount(const RF_Gradient* gradient)
{
    if (!gradient)
        return 0;
    return gradient->stopCount;
}

RF_Color RF_Gradient_GetStopColor(const RF_Gradient* gradient, uint32_t index)
{
    if (!gradient || index >= gradient->stopCount)
        return RF_COLOR_BLACK;

    RF_GradientStop* stop = gradient->stops;
    for (uint32_t i = 0; i < index; ++i)
    {
        stop = stop->next;
        if (!stop)
        {
            RF_LOG_ERROR("RF_Gradient: Invalid stop index");
            return RF_COLOR_BLACK;
        }
    }

    return stop->color;
}

float RF_Gradient_GetStopPosition(const RF_Gradient* gradient, uint32_t index)
{
    if (!gradient || index >= gradient->stopCount)
        return 0.0f;

    RF_GradientStop* stop = gradient->stops;
    for (uint32_t i = 0; i < index; ++i)
    {
        stop = stop->next;
        if (!stop)
        {
            RF_LOG_ERROR("RF_Gradient: Invalid stop index");
            return 0.0f;
        }
    }

    return stop->position;
}

RF_Color RF_Gradient_Evaluate(const RF_Gradient* gradient, float t)
{
    if (!gradient || gradient->stopCount == 0)
        return RF_COLOR_BLACK;

    // Handle spread modes
    switch (gradient->spread)
    {
        case RF_GRADIENT_SPREAD_PAD:
            // t is already clamped
            break;
        case RF_GRADIENT_SPREAD_REFLECT:
            t = fabsf(fmodf(t, 2.0f) - 1.0f);
            break;
        case RF_GRADIENT_SPREAD_REPEAT:
            t = fmodf(t, 1.0f);
            if (t < 0.0f) t += 1.0f;
            break;
    }

    return EvaluateGradientAt(gradient, t);
}

RF_Color RF_Gradient_EvaluateLinear(const RF_Gradient* gradient, float t)
{
    return RF_Gradient_Evaluate(gradient, t);
}

RF_Color RF_Gradient_EvaluateRadial(const RF_Gradient* gradient, float x, float y, float centerX, float centerY, float radius)
{
    if (!gradient || gradient->stopCount == 0)
        return RF_COLOR_BLACK;

    // Calculate distance from center
    float dx = x - centerX;
    float dy = y - centerY;
    float distance = sqrtf(dx * dx + dy * dy);

    // Normalize distance to [0,1] based on radius
    float t = (radius > 0.0f) ? (distance / radius) : 0.0f;

    return RF_Gradient_Evaluate(gradient, t);
}

RF_Color RF_Gradient_EvaluateAngular(const RF_Gradient* gradient, float x, float y, float centerX, float centerY)
{
    if (!gradient || gradient->stopCount == 0)
        return RF_COLOR_BLACK;

    // Calculate angle from center
    float dx = x - centerX;
    float dy = y - centerY;
    float angle = atan2f(dy, dx);

    // Normalize angle to [0,1]
    float t = (angle + RF_PI) / (2.0f * RF_PI);

    return RF_Gradient_Evaluate(gradient, t);
}

RF_Color RF_Gradient_EvaluateDiamond(const RF_Gradient* gradient, float x, float y, float centerX, float centerY, float width, float height)
{
    if (!gradient || gradient->stopCount == 0)
        return RF_COLOR_BLACK;

    // Calculate diamond distance
    float dx = fabsf(x - centerX) / width;
    float dy = fabsf(y - centerY) / height;
    float distance = dx + dy;

    // Normalize to [0,1]
    float t = RF_Clampf(distance, 0.0f, 1.0f);

    return RF_Gradient_Evaluate(gradient, t);
}

RF_Color RFradient_EvaluateConic(const RF_Gradient* gradient, float x, float y, float centerX, float centerY, float startAngle, float endAngle)
{
    if (!gradient || gradient->stopCount == 0)
        return RF_COLOR_BLACK;

    // Calculate angle from center
    float dx = x - centerX;
    float dy = y - centerY;
    float angle = atan2f(dy, dx);

    // Normalize angle to [0,1] based on start and end angles
    float angleRange = endAngle - startAngle;
    float normalizedAngle = angle - startAngle;
    if (angleRange != 0.0f)
    {
        normalizedAngle = fmodf(normalizedAngle, 2.0f * RF_PI);
        if (normalizedAngle < 0.0f) normalizedAngle += 2.0f * RF_PI;
        normalizedAngle /= (2.0f * RF_PI);
    }
    else
    {
        normalizedAngle = 0.0f;
    }

    float t = RF_Clampf(normalizedAngle, 0.0f, 1.0f);
    return RF_Gradient_Evaluate(gradient, t);
}

void RF_Gradient_UpdateCache(RF_Gradient* gradient, uint32_t cacheSize)
{
    if (!gradient)
        return;

    // Free existing cache
    if (gradient->cachedColors)
    {
        free(gradient->cachedColors);
        gradient->cachedColors = NULL;
        gradient->cacheSize = 0;
    }

    // Allocate new cache
    if (cacheSize > 0)
    {
        gradient->cachedColors = (RF_Color*)malloc(cacheSize * sizeof(RF_Color));
        if (gradient->cachedColors)
        {
            gradient->cacheSize = cacheSize;
            
            // Precompute cache values
            for (uint32_t i = 0; i < cacheSize; ++i)
            {
                float t = (float)i / (float)(cacheSize - 1);
                gradient->cachedColors[i] = RF_Gradient_Evaluate(gradient, t);
            }
            
            gradient->isDirty = false;
            RF_LOG_DEBUG("RF_Gradient: Updated cache with ", cacheSize, " entries");
        }
        else
        {
            RF_LOG_ERROR("RF_Gradient: Failed to allocate cache");
        }
    }
}

RF_Color RF_Gradient_GetCachedColor(const RF_Gradient* gradient, uint32_t index, uint32_t cacheSize)
{
    if (!gradient || !gradient->cachedColors || index >= gradient->cacheSize || index >= cacheSize)
        return RF_COLOR_BLACK;

    return gradient->cachedColors[index];
}

bool RF_Gradient_IsDirty(const RF_Gradient* gradient)
{
    if (!gradient)
        return false;
    return gradient->isDirty;
}

void RF_Gradient_MarkDirty(RF_Gradient* gradient)
{
    if (!gradient)
        return;
    gradient->isDirty = true;
}

void RF_Gradient_Print(const RF_Gradient* gradient)
{
    if (!gradient)
    {
        RF_LOG_INFO("Gradient: NULL");
        return;
    }

    RF_LOG_INFO("=== Gradient Information ===");
    RF_LOG_INFO("Type: ", (int)gradient->type);
    RF_LOG_INFO("Spread: ", (int)gradient->spread);
    RF_LOG_INFO("Interpolation: ", (int)gradient->interpolation);
    RF_LOG_INFO("Stop Count: ", gradient->stopCount);
    RF_LOG_INFO("Is Dirty: ", gradient->isDirty ? "Yes" : "No");
    RF_LOG_INFO("Cache Size: ", gradient->cacheSize);

    if (gradient->stopCount > 0)
    {
        RF_LOG_INFO("Stops:");
        RF_GradientStop* stop = gradient->stops;
        uint32_t index = 0;
        while (stop)
        {
            RF_LOG_INFO("  ", index, ": Position=", stop->position, 
                        " Color=0x", RF_Color_ToHex(&stop->color));
            stop = stop->next;
            index++;
        }
    }

    RF_LOG_INFO("========================");
}

void RF_Gradient_PrintStops(const RF_Gradient* gradient)
{
    if (!gradient)
    {
        RF_LOG_INFO("Gradient: NULL");
        return;
    }

    RF_LOG_INFO("Gradient Stops:");
    if (gradient->stopCount == 0)
    {
        RF_LOG_INFO("  No stops");
        return;
    }

    RF_GradientStop* stop = gradient->stops;
    uint32_t index = 0;
    while (stop)
    {
        RF_LOG_INFO("  ", index, ": Position=", stop->position, 
                    " Color=0x", RF_Color_ToHex(&stop->color));
        stop = stop->next;
        index++;
    }
}

// Preset gradients
RF_Gradient* RF_Gradient_CreateRainbow()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, &RF_COLOR_RED, 0.0f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_YELLOW, 0.17f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_GREEN, 0.33f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_CYAN, 0.5f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_BLUE, 0.67f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_MAGENTA, 0.83f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_RED, 1.0f);

    RF_LOG_INFO("RF_Gradient: Created rainbow gradient");
    return gradient;
}

RF_Gradient* RF_Gradient_CreateFire()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, &RF_COLOR_BLACK, 0.0f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(1.0f, 0.0f, 0.0f, 1.0f), 0.2f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(1.0f, 0.5f, 0.0f, 1.0f), 0.4f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(1.0f, 1.0f, 0.0f, 1.0f), 0.6f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(1.0f, 1.0f, 0.5f, 1.0f), 0.8f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_WHITE, 1.0f);

    RF_LOG_INFO("RF_Gradient: Created fire gradient");
    return gradient;
}

RF_Gradient* RF_Gradient_CreateOcean()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, RF_Color_Create(0.0f, 0.2f, 0.4f, 1.0f), 0.0f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.0f, 0.4f, 0.8f, 1.0f), 0.3f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.0f, 0.6f, 0.8f, 1.0f), 0.6f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.0f, 0.8f, 1.0f, 1.0f), 1.0f);

    RF_LOG_INFO("RF_Gradient: Created ocean gradient");
    return gradient;
}

RF_Gradient* RF_Gradient_CreateForest()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, RF_Color_Create(0.1f, 0.2f, 0.1f, 1.0f), 0.0f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.2f, 0.4f, 0.1f, 1.0f), 0.3f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.3f, 0.6f, 0.2f, 1.0f), 0.6f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.2f, 0.8f, 0.3f, 1.0f), 1.0f);

    RF_LOG_INFO("RF_Gradient: Created forest gradient");
    return gradient;
}

RF_Gradient* RF_Gradient_CreateSunset()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, RF_Color_Create(0.2f, 0.4f, 0.8f, 1.0f), 0.0f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.8f, 0.6f, 0.2f, 1.0f), 0.3f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(1.0f, 0.8f, 0.2f, 1.0f), 0.5f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(1.0f, 0.4f, 0.1f, 1.0f), 0.7f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.8f, 0.2f, 0.1f, 1.0f), 1.0f);

    RF_LOG_INFO("RF_Gradient: Created sunset gradient");
    return gradient;
}

RF_Gradient* RF_Gradient_CreateGrayscale()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, &RF_COLOR_BLACK, 0.0f);
    RF_Gradient_AddStop(gradient, &RF_COLOR_WHITE, 1.0f);

    RF_LOG_INFO("RF_Gradient: Created grayscale gradient");
    return gradient;
}

RF_Gradient* RF_Gradient_CreateSepia()
{
    RF_Gradient* gradient = RF_Gradient_Create(RF_GRADIENT_LINEAR);
    if (!gradient)
        return NULL;

    RF_Gradient_AddStop(gradient, RF_Color_Create(0.1f, 0.1f, 0.1f, 1.0f), 0.0f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.4f, 0.3f, 0.2f, 1.0f), 0.5f);
    RF_Gradient_AddStop(gradient, RF_Color_Create(0.7f, 0.5f, 0.3f, 1.0f), 1.0f);

    RF_LOG_INFO("RF_Gradient: Created sepia gradient");
    return gradient;
}

// Advanced gradient functions
RF_Gradient* RF_Gradient_Clone(const RF_Gradient* source)
{
    if (!source)
        return NULL;

    RF_Gradient* gradient = RF_Gradient_Create(source->type);
    if (!gradient)
        return NULL;

    // Copy all stops
    RF_GradientStop* sourceStop = source->stops;
    while (sourceStop)
    {
        RF_Gradient_AddStop(gradient, &sourceStop->color, sourceStop->position);
        sourceStop = sourceStop->next;
    }

    // Copy properties
    gradient->spread = source->spread;
    gradient->interpolation = source->interpolation;

    RF_LOG_INFO("RF_Gradient: Cloned gradient");
    return gradient;
}

bool RF_Gradient_Equals(const RF_Gradient* a, const RF_Gradient* b)
{
    if (!a || !b)
        return false;

    if (a->type != b->type || a->spread != b->spread || a->interpolation != b->interpolation)
        return false;

    if (a->stopCount != b->stopCount)
        return false;

    // Compare stops
    RF_GradientStop* stopA = a->stops;
    RF_GradientStop* stopB = b->stops;
    
    while (stopA && stopB)
    {
        if (!RF_Color_Equals(&stopA->color, &stopB->color) ||
            fabsf(stopA->position - stopB->position) > RF_COLOR_EPSILON)
        {
            return false;
        }
        stopA = stopA->next;
        stopB = stopB->next;
    }

    return true;
}

void RF_Gradient_InvertColors(RF_Gradient* gradient)
{
    if (!gradient)
        return;

    RF_GradientStop* stop = gradient->stops;
    while (stop)
    {
        stop->color = RF_Color_Invert(&stop->color);
        stop = stop->next;
    }

    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Inverted all colors");
}

void RF_Gradient_AdjustBrightness(RF_Gradient* gradient, float factor)
{
    if (!gradient)
        return;

    RF_GradientStop* stop = gradient->stops;
    while (stop)
    {
        stop->color = RF_Color_Brightness(&stop->color, factor);
        stop = stop->next;
    }

    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Adjusted brightness by ", factor);
}

void RF_Gradient_AdjustContrast(RF_Gradient* gradient, float factor)
{
    if (!gradient)
        return;

    RF_GradientStop* stop = gradient->stops;
    while (stop)
    {
        stop->color = RF_Color_Contrast(&stop->color, factor);
        stop = stop->next;
    }

    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Adjusted contrast by ", factor);
}

void RF_Gradient_AdjustSaturation(RF_Gradient* gradient, float factor)
{
    if (!gradient)
        return;

    RF_GradientStop* stop = gradient->stops;
    while (stop)
    {
        stop->color = RF_Color_Saturate(&stop->color, factor);
        stop = stop->next;
    }

    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Adjusted saturation by ", factor);
}

void RF_Gradient_HueShift(RF_Gradient* gradient, float degrees)
{
    if (!gradient)
        return;

    RF_GradientStop* stop = gradient->stops;
    while (stop)
    {
        stop->color = RF_Color_HueShift(&stop->color, degrees);
        stop = stop->next;
    }

    gradient->isDirty = true;
    RF_LOG_DEBUG("RF_Gradient: Hue shifted by ", degrees, " degrees");
}

// Utility function implementations
static RF_GradientStop* CreateGradientStop(const RF_Color* color, float position)
{
    RF_GradientStop* stop = (RF_GradientStop*)malloc(sizeof(RF_GradientStop));
    if (!stop)
        return NULL;

    stop->color = *color;
    stop->position = position;
    stop->next = NULL;
    return stop;
}

static void DestroyGradientStop(RF_GradientStop* stop)
{
    if (stop)
    {
        free(stop);
    }
}

static void SortGradientStops(RF_Gradient* gradient)
{
    if (!gradient || gradient->stopCount <= 1)
        return;

    // Simple bubble sort for now (could be optimized)
    bool sorted;
    do
    {
        sorted = true;
        RF_GradientStop* current = gradient->stops;
        while (current && current->next)
        {
            if (current->position > current->next->position)
            {
                // Swap positions
                float tempPos = current->position;
                current->position = current->next->position;
                current->next->position = tempPos;

                // Swap colors
                RF_Color tempColor = current->color;
                current->color = current->next->color;
                current->next->color = tempColor;

                sorted = false;
            }
            current = current->next;
        }
    } while (!sorted);
}

static RF_Color InterpolateColor(const RF_Color* a, const RF_Color* b, float t, RF_ColorInterpolation interpolation)
{
    t = RF_Clampf(t, 0.0f, 1.0f);

    switch (interpolation)
    {
        case RF_COLOR_INTERPOLATION_LINEAR:
            return RF_Color_Create(
                a->r + (b->r - a->r) * t,
                a->g + (b->g - a->g) * t,
                a->b + (b->b - a->b) * t,
                a->a + (b->a - a->a) * t
            );

        case RF_COLOR_INTERPOLATION_COSINE:
        {
            float cos_t = (1.0f - cosf(t * RF_PI)) * 0.5f;
            return RF_Color_Create(
                a->r + (b->r - a->r) * cos_t,
                a->g + (b->g - a->g) * cos_t,
                a->b + (b->b - a->b) * cos_t,
                a->a + (b->a - a->a) * cos_t
            );
        }

        case RF_COLOR_INTERPOLATION_SMOOTHSTEP:
        {
            float smooth_t = t * t * (3.0f - 2.0f * t);
            return RF_Color_Create(
                a->r + (b->r - a->r) * smooth_t,
                a->g + (b->g - a->g) * smooth_t,
                a->b + (b->b - a->b) * smooth_t,
                a->a + (b->a - a->a) * smooth_t
            );
        }

        case RF_COLOR_INTERPOLATION_SPLINE:
            // Simple cubic Hermite spline interpolation
            // For now, fall back to linear
            return InterpolateColor(a, b, t, RF_COLOR_INTERPOLATION_LINEAR);

        default:
            return InterpolateColor(a, b, t, RF_COLOR_INTERPOLATION_LINEAR);
    }
}

static RF_Color EvaluateGradientAt(const RF_Gradient* gradient, float t)
{
    if (!gradient || gradient->stopCount == 0)
        return RF_COLOR_BLACK;

    // Clamp t to [0,1]
    t = RF_Clampf(t, 0.0f, 1.0f);

    // If only one stop, return that color
    if (gradient->stopCount == 1)
    {
        return gradient->stops->color;
    }

    // Find surrounding stops
    RF_GradientStop* prevStop = NULL;
    RF_GradientStop* nextStop = NULL;

    RF_GradientStop* current = gradient->stops;
    while (current)
    {
        if (current->position <= t)
        {
            prevStop = current;
        }
        if (current->position >= t)
        {
            nextStop = current;
            break;
        }
        current = current->next;
    }

    // Handle edge cases
    if (!prevStop)
    {
        return nextStop ? nextStop->color : RF_COLOR_BLACK;
    }
    if (!nextStop)
    {
        return prevStop->color;
    }

    // If stops are at the same position, return the first one
    if (fabsf(prevStop->position - nextStop->position) < RF_COLOR_EPSILON)
    {
        return prevStop;
    }

    // Interpolate between stops
    float localT = (t - prevStop->position) / (nextStop->position - prevStop->position);
    return InterpolateColor(&prevStop->color, &nextStop->color, localT, gradient->interpolation);
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
const float RF_COLOR_EPSILON = 0.0001f;