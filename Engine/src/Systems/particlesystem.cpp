#include "Engine/Systems/particlesystem.h"

#include <algorithm>
#include <random>

ParticleSystem::ParticleSystem() {}

void ParticleSystem::updateWorld(GameWorld& world, float dt) {
    // Particles should move over time
    moveParticles();
}

std::vector<Particle>& ParticleSystem::getParticles() {
    return particles;
}

void ParticleSystem::add(const Particle& particle) {
    particles.push_back(particle);
}

int ParticleSystem::numParticles() const {
    return particles.size();
}

void ParticleSystem::moveParticles() {
    for (auto& p : particles) {
        p.pos += p.vel;
        p.vel *= 0.985f; // Slow down particles over time (more natural)
        p.lifetime--;

        if (p.type == ParticleType::STREAMER) {
            p.vel.y -= 0.0006f;
        } else if (p.type == ParticleType::BALLISTIC) {
            p.vel.y += 0.0003f;
        }
    }
    // Lifetime = 0, delete particle
    particles.erase(std::remove_if(particles.begin(), particles.end(),
                        [](const Particle& p) { return p.lifetime <= 0; }), particles.end());
}

void ParticleSystem::addMagicBurst(glm::vec3 center, bool float_down, int amount) {
    static std::mt19937 gen(std::random_device{}());

    std::uniform_real_distribution<float> dist_xz(-0.05f, 0.05f);
    std::uniform_real_distribution<float> dist_y(-0.5f, 2.0f);
    std::uniform_real_distribution<float> dist_down_y(-0.002f, 0.001f);
    std::uniform_real_distribution<float> dist_up_y(0.001f, 0.004f);
    std::uniform_int_distribution<int> color(0, 2);
    std::uniform_int_distribution<int> type(0, 1);

    for (int i = 0; i < amount; i++) {
        Particle p;
        p.pos = center + glm::vec3(dist_xz(gen), dist_y(gen), dist_xz(gen));

        if (float_down) {
            p.vel = glm::vec3(dist_xz(gen), dist_up_y(gen), dist_xz(gen));
            p.type = ParticleType::STREAMER;
        } else {
            p.vel = glm::vec3(dist_xz(gen), dist_down_y(gen), dist_xz(gen));
            p.type = ParticleType::BALLISTIC;
        }

        int c = color(gen);
        Color color;
        if (c == 0) {
            color = {0.6f, 0.2f, 1.0f, 1.0f}; // purple
        } else if (c == 1) {
            color = {0.2f, 0.6f, 1.0f, 1.0f}; // blue
        } else {
            color = {1.0f, 0.4f, 0.9f, 1.0f}; // pink
        }

        p.color = color;
        p.size = 0.08f;
        p.lifetime = 600;
        add(p);
    }
}
