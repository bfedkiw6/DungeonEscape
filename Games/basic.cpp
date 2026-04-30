#include "basic.h"

#include <Engine/window.h>
#include <Engine/Graphics/graphics.h>
#include <Engine/Collider.h>
#include <Engine/collision/CylinderCollider.h>
#include <Engine/collision/SphereCollider.h>
#include <Engine/Systems/aisystem.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <random>

gl::DrawMaterial player_mat;
std::mt19937 gen(std::random_device{}());

BasicGame::BasicGame() {
    auto load_start = std::chrono::high_resolution_clock::now();

    screen.setType(ScreenType::MAINMENU);

    cam = std::make_unique<gl::Camera>();
    cam->setPosition(glm::vec3(0.0f, cam->getHeight(), 0.0f));
    cam->setHorizAngle(200.0f);
    cam->setVertAngle(-45.0f);
    cam->setLook(cam->calcNewLook());

    // Setting up each system (they each rely on camera so pass it in)
    camera_system.setCamera(cam.get());
    world.addSystem(&camera_system);

    draw_system.setCamera(cam.get());
    draw_system.setParticleSystem(&particle_system);
    draw_system.makeLights();
    world.addSystem(&draw_system);

    character_system.setCamera(cam.get());
    character_system.setPlayerWidth(1.0f);
    character_system.setFloorWidth(floor_size);
    world.addSystem(&character_system);

    world.addSystem(&collision_system);
    world.addSystem(&obj_system);
    world.addSystem(&particle_system);
    world.addSystem(&animation_system);

    map_mesh = gl::Mesh::loadStaticMesh("resources/models/Map/map.obj");
    createObjects();

    auto load_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);
    std::cout << "[DEBUG] Game load time: " << duration.count() << " ms\n";
}

void BasicGame::createObjects() {
    createPlayer();
    createGuard();
    createMap();
    createWalls();
    createMagicSpots();
}

void BasicGame::createPlayer() {
    player_obj = world.addGameObject();
    player_obj->type = ObjectType::PLAYER;
    player = gl::Mesh::getLoadedShape("cylinder");

    player_obj->addDrawableComp();
    player_obj->getDrawableComp()->shape = player;
    player_obj->getDrawableComp()->visible = false;

    player_obj->addTransformComp();
    glm::vec3 player_pos = glm::vec3(11.5118f, 0.0f, 29.7553f);
    player_obj->getTransformComp()->pos = player_pos;
    player_obj->getTransformComp()->scale = glm::vec3(3.0f, 3.0f, 3.0f);

    player_obj->addCollisionComp();
    player_obj->getCollisionComp()->radius = 0.3f;
    player_obj->getCollisionComp()->height = 1.0f;
    player_obj->getCollisionComp()->shape = CollisionShape::CYLINDER;

    character_system.addGameObject(player_obj);
    camera_system.addGameObject(player_obj);
    draw_system.addGameObject(player_obj);
    collision_system.addGameObject(player_obj);
}

void BasicGame::createGuard() {
    guard_obj = world.addGameObject();

    guard_obj->addTransformComp();
    TransformComponent* gt = guard_obj->getTransformComp();
    gt->pos = glm::vec3(-16.6, -3.0, 16.25);
    gt->scale = glm::vec3(2.5f, 2.5f, 2.5f);
    gt->rotate = glm::rotate(gt->rotate, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    guard_obj->addCollisionComp();
    CollisionComponent* gc = guard_obj->getCollisionComp();
    gc->radius = 0.5f; // TODO: adjust
    gc->height = 1.0f; // TODO: adjust
    gc->shape = CollisionShape::CYLINDER;

    guard_obj->addSkinnedMeshComp();
    SkinnedMeshComponent* gs = guard_obj->getSkinnedMeshComp();
    gs->mesh = gl::Mesh::loadSkinnedMesh("resources/models/Guard/Waving.fbx");
    if (gs->mesh) {
        gs->mesh->skeleton.setCurrentAnimation(0);
    }

    draw_system.addGameObject(guard_obj);
    collision_system.addGameObject(guard_obj);
    animation_system.addGameObject(guard_obj);
}


void BasicGame::createMap() {
    map_obj = world.addGameObject();
    map_obj->type = ObjectType::MAP;

    map_obj->addDrawableComp();
    map_obj->getDrawableComp()->mesh = map_mesh;
    map_obj->getDrawableComp()->visible = true;

    map_obj->addTransformComp();
    TransformComponent* trans = map_obj->getTransformComp();
    trans->pos = glm::vec3(0.0f, -18.0f, 8.0f);
    trans->scale = glm::vec3(0.01f);

    draw_system.addGameObject(map_obj);
}

void BasicGame::createWalls() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> scales;
    int num_walls = 11;
    getWallTransforms(positions, scales);
    //gl::DrawShape* wall_shape = gl::Mesh::getLoadedShape("cube");

    for (int i = 0; i < num_walls; i++) {
        GameObject* wall = world.addGameObject();
        wall->type = ObjectType::WALLS;

        /*wall->addDrawableComp();
        wall->getDrawableComp()->shape = wall_shape;
        draw_system.addGameObject(wall);*/

        wall->addTransformComp();
        wall->getTransformComp()->pos = positions[i];
        wall->getTransformComp()->scale = scales[i];

        wall->addCollisionComp();
        wall->getCollisionComp()->shape = CollisionShape::BOX;
        wall->getCollisionComp()->radius = scales[i].z;
        wall->getCollisionComp()->height = scales[i].y;

        collision_system.addGameObject(wall);
    }
}

void BasicGame::getWallTransforms(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& scales) {
    positions.push_back(glm::vec3(-18.9354, 0.75, 20.0));
    scales.push_back(glm::vec3(2.5, 6.0, 32.0));

    positions.push_back(glm::vec3(7.0, 0.75, 33.854));
    scales.push_back(glm::vec3(50.0, 6.0, 2.5));

    positions.push_back(glm::vec3(-5.0, 0.75, -4.74815));
    scales.push_back(glm::vec3(30.0, 6.0, 2.5));

    positions.push_back(glm::vec3(22.0, 0.75, -4.85759));
    scales.push_back(glm::vec3(17.0, 6.0, 2.5));

    positions.push_back(glm::vec3(31.3189, 0.75, 19.0));
    scales.push_back(glm::vec3(2.5, 6.0, 30.0));

    positions.push_back(glm::vec3(31.0641, 0.75, -16.4502));
    scales.push_back(glm::vec3(2.5, 6.0, 34.0));

    positions.push_back(glm::vec3(-18.9354, 0.75, -16.0396));
    scales.push_back(glm::vec3(2.5, 6.0, 34.0));

    positions.push_back(glm::vec3(25.0, 0.75, -31.0));
    scales.push_back(glm::vec3(90.0, 6.0, 2.5));

    positions.push_back(glm::vec3(64.7459, 0.75, -7.62769));
    scales.push_back(glm::vec3(2.5, 6.0, 50.0));

    positions.push_back(glm::vec3(47.0, 0.75, 17.1381));
    scales.push_back(glm::vec3(34.0, 6.0, 2.5));

    // Wall blocking exit door - delete when all gems found
    positions.push_back(glm::vec3(-18.9354, 0.75, 2.25828));
    scales.push_back(glm::vec3(2.5, 6.0, 5.0));
}

void BasicGame::createMagicSpots() {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> height_fall(5.5f, 7.0f);

    // By the exit door
    magic_spots.push_back(MagicVal{glm::vec3(-17.9502, height_fall(gen), 2.4627), true});
    /*magic_spots.push_back(MagicVal{glm::vec3(-17.9502, height_fall(gen), 6.1434), true});
    magic_spots.push_back(MagicVal{glm::vec3(-17.9502, height_fall(gen), -1.30808), true});

    // By lantern
    magic_spots.push_back(MagicVal{glm::vec3(28.6281, -5.0, -14.02), false});
    magic_spots.push_back(MagicVal{glm::vec3(28.6281, -5.0, -19.85), false});
    magic_spots.push_back(MagicVal{glm::vec3(28.6281, -5.0, -16.61), false});

    // By brooms
    magic_spots.push_back(MagicVal{glm::vec3(59.2251, 6.0, -29.0077), true});
    magic_spots.push_back(MagicVal{glm::vec3(61.2519, -5.0, -27.5818), false});
    magic_spots.push_back(MagicVal{glm::vec3(62.8983, 6.0, -25.4046), true});*/

    // TODO: possibly add near places where the puzzles should go as a clue
}

void BasicGame::draw() {
    if (screen.getType() != ScreenType::GAME && screen.getType() != ScreenType::GUARD) {
        gl::Graphics::clearScreen(glm::vec3(0.1f, 0.1f, 0.1f));
        screen.draw();
        return;
    }
    draw_system.updateWorld(world, 0.0f);
    screen.draw();
}


void BasicGame::update(double delta_time) {
    if (screen.getType() != ScreenType::GAME && screen.getType() != ScreenType::GUARD) {
        // Character shouldn't be able to move in main menu/pause
        return;
    }

    float dt = (float)delta_time;
    player_obj->getDrawableComp()->visible = false;
    collision_system.setOldPosition(character_system.getOldPosition()); // For wall collisions
    screen.setCamPos(camera_system.getCameraPos()); // For screen changes

    // Particles spawn over time
    particle_timer += dt;
    if (particle_timer > 0.75f) {
        for (MagicVal v : magic_spots) {
            particle_system.addMagicBurst(v.pos, v.float_down);
        }
        particle_timer = 0.0f;
    }

    // System updates
    if (screen.getType() != ScreenType::GUARD) {
        // When talking to guard, stop camera & player from moving
        character_system.updateWorld(world, dt);
        camera_system.updateWorld(world, dt);
    }
    collision_system.updateWorld(world, dt);
    obj_system.updateWorld(world, dt);
    particle_system.updateWorld(world, dt);
    animation_system.updateWorld(world, dt);

    // Gem update & check
    /*if (puzzle sucess) {
        screen.incrementGems();
    }*/
    // TEMP: auto win when find all gems - probs change to u need to go put them in the door
    if (found_gems == 3) {
        screen.setType(ScreenType::WIN);
        screen.draw();
    }
}

void BasicGame::resetLevel() {
    screen.resetGems();
    clearObjects();

    glm::vec3 player_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    player_obj->getTransformComp()->pos = glm::vec3(player_pos.x, player_pos.y, player_pos.z);
    character_system.setHorizSpeed(0.0f);
    character_system.reset();
    camera_system.reset();
}

void BasicGame::clearObjects() {
    /*for (GameObject* log : placed_logs) {
        draw_system.removeGameObject(log);
        collision_system.removeGameObject(log);
    }
    placed_logs.clear();

    for (GameObject* coin : placed_coins) {
        draw_system.removeGameObject(coin);
        collision_system.removeGameObject(coin);
    }
    placed_coins.clear();*/
}

void BasicGame::keyEvent(int key, int action) {
    screen.keyEvent(key, action);

    if (action == GLFW_RELEASE) {
        character_system.setKey(key, false);
        return;
    }

    if (screen.getType() == ScreenType::GAME) {
        if (action == GLFW_PRESS){
            character_system.setKey(key, true);
        }
    }
}

void BasicGame::mouseButtonEvent(int button, int action) {

}

void BasicGame::mousePositionEvent(double x_pos, double y_pos) {
    glm::vec2 curr_mouse_pos = glm::vec2((float)x_pos, (float)y_pos);

    // If not in game don't move camera, but should update mouse pos
    if (screen.getType() != ScreenType::GAME) {
        prev_mouse_pos = curr_mouse_pos;
        return;
    }
    // When switching screens, big camera jumps tend to happen --> avoid them
    if (screen.switchScreen) {
        prev_mouse_pos = curr_mouse_pos;
        screen.switchScreen = false;
        return;
    }

    glm::vec2 delta = prev_mouse_pos - curr_mouse_pos;
    camera_system.setMouseDelta(delta);

    prev_mouse_pos = curr_mouse_pos;
}


void BasicGame::mouseScrolledEvent(double x_offset, double y_offset) {

}

void BasicGame::resizeWindowEvent(int new_width, int new_height) {
    camera_system.resizeWindowEvent(new_width, new_height);
}
