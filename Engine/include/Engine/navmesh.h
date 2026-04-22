#ifndef NAVMESH_H
#define NAVMESH_H

#include "glm/ext/vector_float3.hpp"

struct NavTriangle {
    int v0, v1, v2;
    glm::vec3 center;
    std::vector<int> neighbors;
};

class NavMesh {
public:
    void buildGrid(int min_x, int max_x, int min_z, int max_z, float y);
    int findClosestTriangle(glm::vec3& pos);
    std::vector<int> AStar(int start, int goal);

    std::vector<glm::vec3>& getVertices() { return m_vertices; }
    std::vector<NavTriangle>& getTriangles() { return m_triangles; }

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<NavTriangle> m_triangles;

    void computeCenters();
    void buildAdjacency();
};

#endif // NAVMESH_H
