
file(GLOB_RECURSE ENGINE_SOURCE_FILES src/*.cpp)
add_library(CS1950UEngine STATIC ${ENGINE_SOURCE_FILES})

target_include_directories(
    CS1950UEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CS1950UEngine PUBLIC
    ${OPENGL_LIBRARIES}
    glew_static
    glm
    assimp
    stb
    imgui
    raudio
    system
)

set(CS1950UEngine_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Platform-specific settings
if(WIN32)
    add_compile_definitions(GLEW_STATIC)
    target_link_libraries(CS1950UEngine PUBLIC
            opengl32
            glu32
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(CS1950UEngine PUBLIC GL)
endif()
