#ifndef OBJECTCONTROLLERSYSTEM_H
#define OBJECTCONTROLLERSYSTEM_H

#include "system.h"
#include "Engine/Graphics/camera.h"

class ObjectControllerSystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;

private:
};

#endif // CHARACTERCONTROLLERSYSTEM_H
