#pragma once

#include <Engine/game.h>
#include <Engine/screen.h>
#include "Engine/Graphics/camera.h"
#include "Engine/Graphics/mesh.h"
#include "Engine/transform.h"

#include <Engine/gameobject.h>
#include <Engine/gameworld.h>
#include <Engine/navmesh.h>
#include <Engine/Systems/drawsystem.h>
#include <Engine/Systems/camerasystem.h>
#include <Engine/Systems/charactercontrollersystem.h>
#include <Engine/Systems/collisionsystem.h>
#include <Engine/Systems/objectcontrollersystem.h>
#include <Engine/Systems/aisystem.h>
#include <Engine/Systems/particlesystem.h>
#include <Engine/Systems/animationsystem.h>

class BasicGame: public Game {

public:
    BasicGame();
    virtual ~BasicGame() = default;
    virtual void draw() override;

    virtual void update(double delta_time) override;

    virtual void keyEvent(int key, int action) override;
    virtual void mouseButtonEvent(int button, int action) override;
    virtual void mousePositionEvent(double x_pos, double y_pos) override;
    virtual void mouseScrolledEvent(double x_offset, double y_offset) override;
    virtual void resizeWindowEvent(int new_width, int new_height) override;

private:
    // Object creation
    void createObjects();
    void createPlayer();
    void createGuard();
    void createMap();

    void createWalls();
    void getWallTransforms(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& scales);

    struct MagicVal {
        glm::vec3 pos;
        bool float_down;
    };
    std::vector<MagicVal> magic_spots;
    float particle_timer = 4.0f;
    void createMagicSpots();

    void resetLevel();
    void clearObjects();

    // World & Systems
    GameWorld world;
    DrawSystem draw_system;
    CameraSystem camera_system;
    CharacterControllerSystem character_system;
    CollisionSystem collision_system;
    ObjectControllerSystem obj_system;
    ParticleSystem particle_system;
    AnimationSystem animation_system;

    // Objects & Meshes
    gl::DrawMesh* map_mesh;
    GameObject* map_obj;

    GameObject* player_obj = nullptr;
    gl::DrawShape* player;
    float player_width;

    GameObject* guard_obj = nullptr;

    // Floor
    gl::DrawShape* floor_tile;
    int floor_size = 5;
    int floor_size_forward = 500;

    // Rand
    Screen screen;
    ScreenType screen_type;
    std::unique_ptr<gl::Camera> cam;
    glm::vec2 prev_mouse_pos = glm::vec2(0.0f);

    int found_gems = 0;
};
