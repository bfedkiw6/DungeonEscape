cmake_minimum_required(VERSION 3.14)
project(Dungeon_Escape) # TODO Enter your project name here

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)

# ============================================================================
# External Dependencies Configuration
# ============================================================================
# Set which dependencies to enable
set(DEP_GLFW ON "Build GLFW")
set(DEP_GLM ON "Build GLM")
set(DEP_GLEW ON "Build GLEW")
set(DEP_FREETYPE ON "Build FreeType")
set(DEP_STB ON "Include STB headers")
set(DEP_IMGUI ON "Build Dear ImGui")
set(DEP_ASSIMP ON "Build ASSIMP")
set(DEP_RAUDIO ON "Build raylib audio module")

# Path to dependencies directory
set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")
add_subdirectory(${DEPENDENCIES_DIR} Dependencies EXCLUDE_FROM_ALL)

# Set the root directory for resources
set(PROJECT_ROOTDIR "${CMAKE_CURRENT_SOURCE_DIR}")
    # Defines PROJECT_ROOT macro for use in source files
    add_compile_definitions(PROJECT_ROOT="${PROJECT_ROOTDIR}")

add_subdirectory(Engine)
add_subdirectory(Games)
