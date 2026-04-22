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
    void createObjects();
    void createPlayer();
    void createEnemy();
    void createLog();
    void createCoins();
    void createEnvironment();
    bool findObjectOverlap(GameObject* a, GameObject* b);

    void resetLevel();
    void clearObjects();

    GameWorld world;
    DrawSystem draw_system;
    CameraSystem camera_system;
    CharacterControllerSystem character_system;
    CollisionSystem collision_system;
    ObjectControllerSystem obj_system;

    Screen screen;
    ScreenType screen_type;

    std::unique_ptr<gl::Camera> cam;

    gl::DrawMesh* skull_mesh;
    GameObject* enemy_obj = nullptr;
    NavMesh nav_mesh;
    std::vector<int> enemy_path;
    int enemy_path_index = 0;
    float enemy_speed = 4.0f;


    GameObject* player_obj = nullptr;
    gl::DrawMesh* frog_mesh;
    gl::DrawShape* player;
    float player_width;

    gl::DrawShape* floor_tile;
    int floor_size = 5;
    int floor_size_forward = 500;
    gl::DrawShape* wall;
    GameObject* sky_obj = nullptr;
    gl::DrawShape* sky_dome;

    gl::DrawShape* log;
    std::vector<GameObject*> placed_logs;

    gl::DrawShape* coin;
    std::vector<GameObject*> placed_coins;

    glm::vec2 prev_mouse_pos = glm::vec2(0.0f);
};
