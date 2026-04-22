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
gl::DrawMaterial grass_mat;
gl::DrawMaterial wood_mat;
gl::DrawMaterial water_mat;
gl::DrawMaterial rock_mat;
gl::DrawMaterial sky_mat;
std::mt19937 gen(std::random_device{}());

BasicGame::BasicGame() {
    auto load_start = std::chrono::high_resolution_clock::now();

    screen.setType(ScreenType::MAINMENU);

    cam = std::make_unique<gl::Camera>();
    cam->setPosition(glm::vec3(0.0f, cam->getHeight(), 0.0f));

    // Setting up each system (they each rely on camera so pass it in)
    camera_system.setCamera(cam.get());
    world.addSystem(&camera_system);

    draw_system.setCamera(cam.get());
    world.addSystem(&draw_system);

    character_system.setCamera(cam.get());
    character_system.setPlayerWidth(1.0f);
    world.addSystem(&character_system);

    world.addSystem(&collision_system);
    world.addSystem(&obj_system);

    grass_mat.textures = gl::Material::loadTexture("resources/images/grass.png");
    water_mat.textures = gl::Material::loadTexture("resources/images/water.png");
    wood_mat.textures = gl::Material::loadTexture("resources/images/wood.png");
    rock_mat.textures = gl::Material::loadTexture("resources/images/rock.png");
    sky_mat.textures = gl::Material::loadTexture("resources/images/sky.png");
    frog_mesh = gl::Mesh::loadStaticMesh("resources/Models/Frog/Ceramic-frog_low-poly.obj");
    skull_mesh = gl::Mesh::loadStaticMesh("resources/Models/Skull/12140_Skull_v3_L2.obj");
    createObjects();

    auto load_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);
    std::cout << "[DEBUG] Game load time: " << duration.count() << " ms\n";
}

void BasicGame::createObjects() {
    createPlayer();
    createEnvironment();
}

void BasicGame::createPlayer() {
    player_obj = world.addGameObject();
    player_obj->type = ObjectType::PLAYER;
    player = gl::Mesh::getLoadedShape("cylinder");

    player_obj->addDrawableComp();
    player_obj->getDrawableComp()->shape = player;
    player_obj->getDrawableComp()->visible = false;

    player_obj->addTransformComp();
    glm::vec3 player_pos = glm::vec3(0.0f, 0.0f, 0.0f);
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

void BasicGame::createEnvironment() {
    // Floor
    floor_tile = gl::Mesh::getLoadedShape("quad");
    character_system.setFloorWidth(floor_size);
    for (int x = -floor_size; x < floor_size; x++) {
        for (int z = -floor_size; z < floor_size_forward; z++) {
            GameObject* floor_obj = world.addGameObject();
            floor_obj->addDrawableComp();
            floor_obj->getDrawableComp()->shape = floor_tile;
            floor_obj->getDrawableComp()->mat = grass_mat;

            floor_obj->addTransformComp();
            floor_obj->getTransformComp()->pos = glm::vec3((float)x, 0.0f, (float)z);

            draw_system.addGameObject(floor_obj);
        }
        // Ending section
        for (int k = floor_size_forward; k < floor_size_forward + floor_size; k++) {
            GameObject* floor_obj = world.addGameObject();
            floor_obj->addDrawableComp();
            floor_obj->getDrawableComp()->shape = floor_tile;
            floor_obj->getDrawableComp()->mat = water_mat;

            floor_obj->addTransformComp();
            floor_obj->getTransformComp()->pos = glm::vec3((float)x, 0.0f, (float)k);

            draw_system.addGameObject(floor_obj);
        }
    }
    // Walls
    wall = gl::Mesh::getLoadedShape("cube");
    for (int z = -floor_size; z < floor_size_forward + floor_size; z++) {
        GameObject* wall_obj = world.addGameObject();
        wall_obj->addDrawableComp();
        wall_obj->getDrawableComp()->shape = wall;
        wall_obj->getDrawableComp()->mat = rock_mat;
        wall_obj->addTransformComp();
        wall_obj->getTransformComp()->pos = glm::vec3(-floor_size - 1.0f, 0.0f, (float)z);
        wall_obj->getTransformComp()->scale = glm::vec3(1.0f, 4.0f, 1.0f);
        draw_system.addGameObject(wall_obj);

        GameObject* wall_obj2 = world.addGameObject();
        wall_obj2->addDrawableComp();
        wall_obj2->getDrawableComp()->shape = wall;
        wall_obj2->getDrawableComp()->mat = rock_mat;
        wall_obj2->addTransformComp();
        wall_obj2->getTransformComp()->pos = glm::vec3(floor_size, 0.0f, (float)z);
        wall_obj2->getTransformComp()->scale = glm::vec3(1.0f, 4.0f, 1.0f);
        draw_system.addGameObject(wall_obj2);
    }

    // Sky
    sky_dome = gl::Mesh::getLoadedShape("sphere");
    sky_obj = world.addGameObject();
    sky_obj->type = ObjectType::SKY;
    sky_obj->addDrawableComp();
    sky_obj->getDrawableComp()->shape = sky_dome;
    sky_obj->getDrawableComp()->mat = sky_mat;
    sky_obj->addTransformComp();
    TransformComponent* trans = sky_obj->getTransformComp();
    trans->pos = cam->getPosition();
    trans->scale = glm::vec3(200.0f);
    trans->rotate = glm::rotate(trans->rotate,
                                glm::radians(90.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));
    draw_system.addGameObject(sky_obj);
}

bool BasicGame::findObjectOverlap(GameObject* a, GameObject* b) {
    TransformComponent* trans_a = a->getTransformComp();
    CollisionComponent* col_a = a->getCollisionComp();
    TransformComponent* trans_b = b->getTransformComp();
    CollisionComponent* col_b = b->getCollisionComp();

    MTV result;
    if (col_a->shape == CollisionShape::CYLINDER && col_b->shape == CollisionShape::CYLINDER) {
        // Cases: log v log
        CylinderCollider colA(trans_a, col_a->radius, col_a->height);
        CylinderCollider colB(trans_b, col_b->radius, col_b->height);
        result = colA.getMTV(&colB);
    } else if (col_a->shape == CollisionShape::SPHERE && col_b->shape == CollisionShape::SPHERE) {
        // Cases: coin v coin
        SphereCollider colA(trans_a, col_a->radius);
        SphereCollider colB(trans_b, col_b->radius);
        result = colA.getMTV(&colB);
    } else if (col_a->shape == CollisionShape::SPHERE && col_b->shape == CollisionShape::CYLINDER) {
        // Cases: coin v log
        SphereCollider colA(trans_a, col_a->radius);
        CylinderCollider colB(trans_b, col_b->radius, col_b->height);
        result = colA.getMTV(&colB);
    }
    return result.collision;
}

void BasicGame::draw() {
    if (screen.getType() != ScreenType::GAME) {
        gl::Graphics::clearScreen(glm::vec3(0.1f, 0.1f, 0.1f));
        screen.draw();
        return;
    }
    draw_system.updateWorld(world, 0.0f);
    screen.draw();
}


void BasicGame::update(double delta_time) {
    if (screen.getType() != ScreenType::GAME) {
        // Character shouldn't be able to move in main menu/pause
        return;
    }
    float dt = (float)delta_time;
    player_obj->getDrawableComp()->visible = false;

    // Make sure sky is located where the player is
    sky_obj->getTransformComp()->pos = cam->getPosition();

    character_system.updateWorld(world, dt);
    camera_system.updateWorld(world, dt);
    collision_system.updateWorld(world, dt);
    obj_system.updateWorld(world, dt);
}

void BasicGame::resetLevel() {
    screen.resetCoins();
    clearObjects();

    glm::vec3 player_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    player_obj->getTransformComp()->pos = glm::vec3(player_pos.x, player_pos.y + 0.5f, player_pos.z);
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
