# CMakeLists.txt for ImGuizmo
# This file should be placed ONE LEVEL ABOVE the ImGuizmo directory
# Usage: add_subdirectory(path/to/this/directory) and link against "imguizmo"
#
# Directory structure expected:
#   your_project/
#   ├── CMakeLists.txt          <- This file
#   └── ImGuizmo/               <- Clone of https://github.com/CedricGuillemet/ImGuizmo
#       ├── ImGuizmo.cpp
#       ├── ImGuizmo.h
#       └── ...

cmake_minimum_required(VERSION 3.16)
project(imguizmo LANGUAGES CXX)

# Options for component selection
option(IMGUIZMO_BUILD_GRAPH_EDITOR "Include GraphEditor" ON)
option(IMGUIZMO_BUILD_CURVE_EDIT "Include ImCurveEdit" ON)
option(IMGUIZMO_BUILD_GRADIENT "Include ImGradient" ON)
option(IMGUIZMO_BUILD_SEQUENCER "Include ImSequencer" ON)

# Set the path to ImGuizmo source directory
set(IMGUIZMO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo")

# ============================================================================
# Validation
# ============================================================================

# Verify ImGuizmo directory exists
if(NOT EXISTS "${IMGUIZMO_DIR}/ImGuizmo.h")
    message(FATAL_ERROR
            "ImGuizmo source not found at ${IMGUIZMO_DIR}\n"
            "Please clone ImGuizmo into the 'ImGuizmo' subdirectory:\n"
            "  git clone https://github.com/CedricGuillemet/ImGuizmo.git ${IMGUIZMO_DIR}"
    )
endif()

# Verify imgui target exists
if(NOT TARGET imgui)
    message(FATAL_ERROR
            "ImGuizmo requires Dear ImGui.\n"
            "Please ensure the imgui target is available before adding ImGuizmo."
    )
endif()

# ============================================================================
# Create the imguizmo library
# ============================================================================

add_library(imguizmo STATIC)

# Add alias for use with FetchContent or add_subdirectory
add_library(imguizmo::imguizmo ALIAS imguizmo)

# Core source files (required)
target_sources(imguizmo PRIVATE
        "${IMGUIZMO_DIR}/ImGuizmo.cpp"
)

# Core header files (for IDE visibility)
target_sources(imguizmo PRIVATE
        "${IMGUIZMO_DIR}/ImGuizmo.h"
)

# Optional components
if(IMGUIZMO_BUILD_GRAPH_EDITOR)
    target_sources(imguizmo PRIVATE
            "${IMGUIZMO_DIR}/GraphEditor.cpp"
            "${IMGUIZMO_DIR}/GraphEditor.h"
    )
endif()

if(IMGUIZMO_BUILD_CURVE_EDIT)
    target_sources(imguizmo PRIVATE
            "${IMGUIZMO_DIR}/ImCurveEdit.cpp"
            "${IMGUIZMO_DIR}/ImCurveEdit.h"
    )
endif()

if(IMGUIZMO_BUILD_GRADIENT)
    target_sources(imguizmo PRIVATE
            "${IMGUIZMO_DIR}/ImGradient.cpp"
            "${IMGUIZMO_DIR}/ImGradient.h"
    )
endif()

if(IMGUIZMO_BUILD_SEQUENCER)
    target_sources(imguizmo PRIVATE
            "${IMGUIZMO_DIR}/ImSequencer.cpp"
            "${IMGUIZMO_DIR}/ImSequencer.h"
    )
endif()

# Header-only components (always included for IDE visibility)
target_sources(imguizmo PRIVATE
        "${IMGUIZMO_DIR}/ImLightRig.h"
        "${IMGUIZMO_DIR}/ImZoomSlider.h"
)

# Set include directories
target_include_directories(imguizmo PUBLIC
        $<BUILD_INTERFACE:${IMGUIZMO_DIR}>
        $<BUILD_INTERFACE:${IMGUIZMO_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/imguizmo>
)

# Require C++20
target_compile_features(imguizmo PUBLIC cxx_std_20)

# Link to imgui
target_link_libraries(imguizmo PUBLIC imgui)

# ============================================================================
# Installation (optional, disabled by default)
# ============================================================================

option(IMGUIZMO_INSTALL "Generate installation target" OFF)

if(IMGUIZMO_INSTALL)
    include(GNUInstallDirs)

    install(TARGETS imguizmo
            EXPORT imguizmoTargets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(FILES
            "${IMGUIZMO_DIR}/ImGuizmo.h"
            "${IMGUIZMO_DIR}/GraphEditor.h"
            "${IMGUIZMO_DIR}/ImCurveEdit.h"
            "${IMGUIZMO_DIR}/ImGradient.h"
            "${IMGUIZMO_DIR}/ImSequencer.h"
            "${IMGUIZMO_DIR}/ImLightRig.h"
            "${IMGUIZMO_DIR}/ImZoomSlider.h"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/imguizmo
    )

    install(EXPORT imguizmoTargets
            FILE imguizmoTargets.cmake
            NAMESPACE imguizmo::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/imguizmo
    )
endif()

# Print configuration summary
message(STATUS "ImGuizmo Configuration:")
message(STATUS "  C++ Standard: C++20")
message(STATUS "  GraphEditor: ${IMGUIZMO_BUILD_GRAPH_EDITOR}")
message(STATUS "  CurveEdit:   ${IMGUIZMO_BUILD_CURVE_EDIT}")
message(STATUS "  Gradient:    ${IMGUIZMO_BUILD_GRADIENT}")
message(STATUS "  Sequencer:   ${IMGUIZMO_BUILD_SEQUENCER}")
