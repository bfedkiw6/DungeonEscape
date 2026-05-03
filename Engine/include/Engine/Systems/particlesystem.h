#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Engine/Systems/system.h"
#include "glm/ext/vector_float3.hpp"

enum class ParticleType {
    STREAMER,
    BALLISTIC,
    FIREWORK
};

struct Color {
    float r, g, b, a;
};

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    Color color;
    float size;
    int lifetime;
    ParticleType type;
};

class ParticleSystem : public System
{
public:
    ParticleSystem();

    void updateWorld(GameWorld& world, float dt) override;
    std::vector<Particle>& getParticles();
    void add(const Particle& particle);
    int numParticles() const;
    void drawParticles() const;
    void moveParticles();
    void addMagicBurst(glm::vec3 center, bool float_down, int amount);

private:
    std::vector<Particle> particles;
};

#endif // PARTICLESYSTEM_H
