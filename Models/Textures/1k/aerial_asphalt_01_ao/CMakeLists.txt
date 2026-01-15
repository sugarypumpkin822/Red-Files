cmake_minimum_required(VERSION 3.16)
project(TextureJpgRebuild LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Organized source groups
file(GLOB_RECURSE DATA_PART_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/data/parts/*.cpp")

add_library(texture_data
  src/data/texture_parts.cpp
  ${DATA_PART_SOURCES}
)

add_library(texture_io
  src/io/file_write.cpp
)

add_library(texture_core
  src/core/rebuild.cpp
)

add_library(texture_ui
  src/ui/file_dialog.cpp
  src/ui/main_window.cpp
)

target_include_directories(texture_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(texture_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(texture_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(texture_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(texture_core PUBLIC texture_data texture_io)
target_link_libraries(texture_ui PUBLIC texture_core)

add_executable(texture_rebuild_gui WIN32
  src/app/win_main.cpp
)
target_link_libraries(texture_rebuild_gui PRIVATE texture_ui)
