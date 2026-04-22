file(GLOB_RECURSE GAME_SOURCE_FILES main-basic.cpp basic.cpp)
add_executable(BasicGame ${GAME_SOURCE_FILES})

target_include_directories(BasicGame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        CS1950UEngine_INCLUDE
)

target_link_libraries(
        BasicGame
        CS1950UEngine
)

### TODO: ADD FILES FOR OTHER GAMES HERE ###