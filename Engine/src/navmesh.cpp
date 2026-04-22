#include "glm/geometric.hpp"
#include <Engine/navmesh.h>

void NavMesh::buildGrid(int minX, int maxX, int minZ, int maxZ, float y) {
    m_vertices.clear();
    m_triangles.clear();
    int width = maxX - minX;

    // Create grid vertices
    for (int z = minZ; z <= maxZ; z++) {
        for (int x = minX; x <= maxX; x++) {
            m_vertices.push_back(glm::vec3((float)x, y, (float)z));
        }
    }

    // Split grid into two triangles
    for (int z = minZ; z < maxZ; z++) {
        for (int x = minX; x < maxX; x++) {
            // Convert coords to indices --> index = row * width + col (when grid starts at (0,0))
            int v00 = (z - minZ) * (width + 1) + (x - minX);
            int v10 = (z - minZ) * (width + 1) + ((x + 1) - minX);
            int v01 = ((z + 1) - minZ) * (width + 1) + (x - minX);
            int v11 = ((z + 1) - minZ) * (width + 1) + ((x + 1) - minX);

            NavTriangle t1;
            t1.v0 = v00;
            t1.v1 = v10;
            t1.v2 = v11;
            m_triangles.push_back(t1);

            NavTriangle t2;
            t2.v0 = v00;
            t2.v1 = v11;
            t2.v2 = v01;
            m_triangles.push_back(t2);
        }
    }

    computeCenters();
    buildAdjacency();
}

void NavMesh::computeCenters() {
    for (NavTriangle& tri : m_triangles) {
        glm::vec3 a = m_vertices[tri.v0];
        glm::vec3 b = m_vertices[tri.v1];
        glm::vec3 c = m_vertices[tri.v2];
        tri.center = (a + b + c) / 3.0f;
    }
}

void NavMesh::buildAdjacency() {
    for (int i = 0; i < m_triangles.size(); i++) {
        for (int j = i + 1; j < m_triangles.size(); j++) {
            int shared = 0;
            int tri0[3] = { m_triangles[i].v0, m_triangles[i].v1, m_triangles[i].v2 };
            int tri1[3] = { m_triangles[j].v0, m_triangles[j].v1, m_triangles[j].v2 };
            for (int a = 0; a < 3; a++) {
                for (int b = 0; b < 3; b++) {
                    if (tri0[a] == tri1[b]) {
                        shared++;
                    }
                }
            }
            // If 2 vertices are shared, they share an edge
            if (shared == 2) {
                m_triangles[i].neighbors.push_back(j);
                m_triangles[j].neighbors.push_back(i);
            }
        }
    }
}

int NavMesh::findClosestTriangle(glm::vec3& pos) {
    // Start first triangle as closest one
    int closest = 0;
    float smallest_dist = glm::length(m_triangles[0].center - pos);

    // Compare to other triangles
    for (int i = 1; i < m_triangles.size(); i++) {
        float dist = glm::length(m_triangles[i].center - pos);
        if (dist < smallest_dist) {
            closest = i;
            smallest_dist = dist;
        }
    }
    return closest;
}

std::vector<int> NavMesh::AStar(int start, int goal) {
    std::vector<int> path;
    std::vector<int> open_list;
    std::vector<int> closed_list;

    int num_tris = m_triangles.size();
    std::vector<float> g_cost(num_tris, std::numeric_limits<float>::max());
    std::vector<float> h_cost(num_tris, std::numeric_limits<float>::max());
    std::vector<float> f_cost(num_tris, std::numeric_limits<float>::max());
    std::vector<int> parent(num_tris, -1);

    // Add starting node to the open list
    open_list.push_back(start);
    g_cost[start] = 0.0f;
    h_cost[start] = glm::length(m_triangles[start].center - m_triangles[goal].center);
    f_cost[start] = g_cost[start] + h_cost[start];

    while (!open_list.empty()) {
        // Find node w/ lowest F cost from open list
        int C = open_list[0];
        for (int node : open_list) {
            if (f_cost[node] < f_cost[C]) {
                C = node;
            }
        }
        // Move C from open to closed list
        open_list.erase(std::remove(open_list.begin(), open_list.end(), C), open_list.end());
        closed_list.push_back(C);

        // If C = B, stop searching and add all previously search nodes to path
        if (C == goal) {
            int curr = goal;
            while (curr != -1) {
                path.push_back(curr);
                curr = parent[curr];
            }
            // Need path in start to goal order
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Check each neighbor
        for (int N : m_triangles[C].neighbors) {
            if (std::find(closed_list.begin(), closed_list.end(), N) != closed_list.end()) {
                // Skip to next neighbor since N was already searched
                continue;
            }
            if (std::find(open_list.begin(), open_list.end(), N) == open_list.end()) {
                // Need to still search for neighbor --> add to open list
                open_list.push_back(N);
            }
            // Compute F, G, and H costs of N
            float newG = g_cost[C] + glm::length(m_triangles[C].center - m_triangles[N].center);
            float H = glm::length(m_triangles[N].center - m_triangles[goal].center);
            float F = newG + H;

            g_cost[N] = newG;
            h_cost[N] = H;
            f_cost[N] = F;
            parent[N] = C;
        }
    }
    // No path found
    return path;
}
