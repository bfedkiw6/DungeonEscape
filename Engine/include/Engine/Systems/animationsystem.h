#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "system.h"

class AnimationSystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;

private:
};

#endif // ANIMATIONSYSTEM_H
