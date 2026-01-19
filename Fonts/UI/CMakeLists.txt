cmake_minimum_required(VERSION 3.16)
project(RedFilesFontCompiler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /Zi /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /DNDEBUG /EHsc")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/integration)

# Source files for the font compiler
set(FONT_COMPILER_SOURCES
    integration/rf_font_api.cpp
    integration/rf_blood_api.cpp
    integration/rf_integration.cpp
    src/core/rf_font.c
    src/core/rf_font_cache.cpp
    src/core/rf_character_map.cpp
    src/animation/rf_animation.cpp
    src/animation/rf_animation_controller.cpp
    src/animation/rf_animation_state.cpp
    src/blood/core/rf_blood_system.cpp
    src/blood/core/rf_blood_physics.cpp
    src/blood/core/rf_blood_renderer.cpp
    src/blood/behaviors/rf_blood_drip.cpp
    src/blood/behaviors/rf_blood_splash.cpp
    src/blood/behaviors/rf_blood_flow.cpp
    src/blood/emitters/rf_blood_drip_emitter.cpp
    src/blood/emitters/rf_blood_splash_emitter.cpp
    src/effects/rf_chromatic_aberration.cpp
    src/effects/rf_distortion_effect.cpp
    src/effects/rf_fade_effect.cpp
    src/effects/rf_blood_effect.cpp
    src/effects/rf_particle_effect.cpp
    src/effects/rf_shader_effect.cpp
    platform/windows/rf_platform_win.cpp
    platform/windows/rf_window_win.cpp
    platform/windows/rf_input_win.cpp
)

# Create a simple font compiler executable
add_executable(font_compiler
    tools/font_compiler_main.cpp
    ${FONT_COMPILER_SOURCES}
)

# Create font baker executable
add_executable(font_baker
    tools/font_baker/src/main.cpp
    tools/font_baker/src/baker.cpp
    tools/font_baker/src/rasterizer.cpp
    tools/font_baker/src/writer.cpp
    ${FONT_COMPILER_SOURCES}
)

# Link libraries
if(WIN32)
    target_link_libraries(font_compiler 
        kernel32
        user32
        gdi32
        opengl32
    )
    
    target_link_libraries(font_baker
        kernel32
        user32
        gdi32
        opengl32
    )
endif()

# Set output directory
set_target_properties(font_compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(font_baker PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS font_compiler font_baker
    RUNTIME DESTINATION bin
)
