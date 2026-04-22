#pragma once
#include "Engine/collision/ConeCollider.h"

class TriangleCollider;
class ConvexCollider;
class SphereCollider;
class CylinderCollider;
class BoxCollider;
struct MTV;
class Collider;

struct Simplex {
    // TODO - use for constructing polytope with EPA
    // Fill this out with whatever you need. e.g array<glm::vec3, 4> points;
private:
    std::array<glm::vec3, 4> m_points;
    int m_size;

public:
    Simplex() : m_size (0) {}

    Simplex& operator=(std::initializer_list<glm::vec3> list) {
        m_size = 0;
        for (glm::vec3 point : list)
            m_points[m_size++] = point;
        return *this;
    }

    void push_front(glm::vec3 point) {
        m_points = { point, m_points[0], m_points[1], m_points[2] };
        m_size = std::min(m_size + 1, 4);
    }

    glm::vec3& operator[](int i) { return m_points[i]; }
    size_t size() const { return m_size; }

    auto begin() const { return m_points.begin(); }
    auto end() const { return m_points.end() - (4 - m_size); }
};
class GJK {

public:
    static bool detectCollision(const Collider* a, const Collider* b, Simplex& simplex);
    static MTV getMTV(const Collider* a, const Collider* b);

    // Support point functions for different collider types
    static glm::vec3 getBoxSupportPoint(const BoxCollider* box, const glm::vec3& direction);
    static glm::vec3 getCylinderSupportPoint(const CylinderCollider* cylinder, const glm::vec3& direction);
    static glm::vec3 getSphereSupportPoint(const SphereCollider* sphere, const glm::vec3& direction);
    static glm::vec3 getConeSupportPoint(const ConeCollider* cone, const glm::vec3& direction);
    static glm::vec3 getConvexSupportPoint(const ConvexCollider* convex, const glm::vec3& direction);
    static glm::vec3 getTriangleSupportPoint(const TriangleCollider* triangle, const glm::vec3& direction);

    static bool nextSimplex(Simplex& points, glm::vec3& dir);
    static bool line(Simplex& points,  glm::vec3& direction);
    static bool triangle(Simplex& points, glm::vec3& dir);
    static bool tetrahedron(Simplex& points, glm::vec3& dir);

    static MTV collisionResponse(const Collider* a, const Collider* b, Simplex& simplex);
    static std::pair<std::vector<glm::vec4>, size_t> getFaceNormals(const std::vector<glm::vec3>& polytope,
                                                                    const std::vector<size_t>& faces);
    static void addIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges,
                         const std::vector<size_t>& faces,
                         size_t a, size_t b);
};
