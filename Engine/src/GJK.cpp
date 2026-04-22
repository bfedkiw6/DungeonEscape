#include "Engine/GJK.h"
#include "Engine/Collider.h"
#include "Engine/collision/BoxCollider.h"
#include "Engine/collision/CylinderCollider.h"
#include "Engine/collision/SphereCollider.h"
#include "Engine/collision/ConvexCollider.h"
#include "Engine/collision/TriangleCollider.h"
#include <iostream>

bool GJK::detectCollision(const Collider* a, const Collider* b, Simplex& simplex) {
    // Running GJK
    glm::vec3 supportA = a->getSupportPoint(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 supportB = b->getSupportPoint(glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 support = supportA - supportB;

    // Add support point to simplex & make new direction towards origin
    simplex.push_front(support);
    glm::vec3 dir = -support;
    // Add more support points
    while (true) {
        support = a->getSupportPoint(dir) - b->getSupportPoint(-dir);
        if (glm::dot(support, dir) <= 0) {
            // No collision
            return false;
        }
        simplex.push_front(support);
        // Update the simplex when a new point is added successfully
        if (nextSimplex(simplex, dir)) {
            return true;
        }
    }
    return false;
}

MTV GJK::getMTV(const Collider* a, const Collider* b) {
    Simplex simplex;
    if (detectCollision(a, b, simplex)) {
        return collisionResponse(a, b, simplex);
    }
    return {};
}

glm::vec3 GJK::getBoxSupportPoint(const BoxCollider* box, const glm::vec3& direction) {
    // Find in local space first since equation only works for box at origin
    TransformComponent& t = *box->transform;
    glm::vec3 half = t.scale * 0.5f;
    glm::mat4 model = t.getModelMatrix();
    glm::vec3 dir = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));

    glm::vec3 support(dir.x >= 0.0f ? half.x : -half.x,
                      dir.y >= 0.0f ? half.y : -half.y,
                      dir.z >= 0.0f ? half.z : -half.z);

    return glm::vec3(model * glm::vec4(support, 1.0f));
}

glm::vec3 GJK::getCylinderSupportPoint(const CylinderCollider* cylinder, const glm::vec3& direction) {  
    TransformComponent& t = *cylinder->transform;
    glm::mat4 model = t.getModelMatrix();
    glm::vec3 dir = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));

    float sigma = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float half_y = cylinder->height * 0.5f;
    float y = dir.y >= 0.0f ? half_y : -half_y;
    float radius = cylinder->radius;

    glm::vec3 support(0.0f, y, 0.0f);
    if (sigma > 0.0f) {
        support = glm::vec3((radius / sigma) * dir.x, y, (radius / sigma) * dir.z);
    }

    return glm::vec3(model * glm::vec4(support, 1.0f));
}

glm::vec3 GJK::getSphereSupportPoint(const SphereCollider* sphere, const glm::vec3& direction) {
    TransformComponent& t = *sphere->transform;
    glm::mat4 model = t.getModelMatrix();
    glm::vec3 dir = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));
    float radius = sphere->radius;

    glm::vec3 support(0.0f, 0.0f, 0.0f);
    if (glm::length(dir) > 0.0f) {
        support = radius * glm::normalize(dir);
    }

    return glm::vec3(model * glm::vec4(support, 1.0f));
}

glm::vec3 GJK::getConeSupportPoint(const ConeCollider* cone, const glm::vec3& direction) {
    TransformComponent& t = *cone->transform;
    glm::mat4 model = t.getModelMatrix();
    glm::vec3 dir = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));

    glm::vec3 half = t.scale * 0.5f;
    float sigma = sqrt(dir.x * dir.x + dir.z * dir.z);
    float y = half.y;
    float radius = half.x;
    float sin_alpha = radius / sqrt(radius * radius + 2.0f*y * 2.0f*y);

    glm::vec3 support(0.0f, -y, 0.0f);
    if (dir.y > sin_alpha * glm::length(dir)) {
        support = glm::vec3(0.0f, y, 0.0f);
    } else if (sigma > 0) {
        support = glm::vec3((radius / sigma)*dir.x, -y, (radius / sigma)*dir.z);
    }

    return glm::vec3(model * glm::vec4(support, 1.0f));
}

glm::vec3 GJK::getConvexSupportPoint(const ConvexCollider* convex, const glm::vec3& direction) {
    auto points = convex->getPoints();
    TransformComponent& t = *convex->transform;
    glm::mat4 model = t.getModelMatrix();
    glm::vec3 localDir = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));

    glm::vec3 max_point;
    float max_dist = -FLT_MAX;
    for (const glm::vec3& point : points) {
        float distance = glm::dot(point, localDir);
        if (distance > max_dist) {
            max_dist = distance;
            max_point = point;
        }
    }
    return glm::vec3(model * glm::vec4(max_point, 1.0f));
}

glm::vec3 GJK::getTriangleSupportPoint(const TriangleCollider* triangle, const glm::vec3& direction) {
    auto vertices = triangle->getVertices();
    TransformComponent& t = *triangle->transform;
    glm::mat4 model = t.getModelMatrix();
    glm::vec3 localDir = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));

    glm::vec3 max_point;
    float max_dist = -FLT_MAX;
    for (glm::vec3 vertex : vertices) {
        float distance = glm::dot(vertex, direction);
        if (distance > max_dist) {
            max_dist = distance;
            max_point = vertex;
        }
    }
    return glm::vec3(model * glm::vec4(max_point, 1.0f));
}

bool GJK::nextSimplex(Simplex& points, glm::vec3& dir) {
    switch (points.size()) {
    case 2: return line(points, dir);
    case 3: return triangle(points, dir);
    case 4: return tetrahedron(points, dir);
    }
    return false;
}

bool GJK::line(Simplex& points, glm::vec3& dir) {
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 ab = b - a;
    glm::vec3 ao = -a;

    if (glm::dot(ab, ao) > 0) {
        // Vectors are same direction
        dir = glm::cross(glm::cross(ab, ao), ab);
    } else {
        points = { a };
        dir = ao;
    }
    return false;
}

bool GJK::triangle(Simplex& points, glm::vec3& dir) {
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ao = -a;
    glm::vec3 abc = glm::cross(ab, ac);

    if (glm::dot(glm::cross(abc, ac), ao) > 0) {
        if (glm::dot(ac, ao) > 0) {
            points = { a, c };
            dir = glm::cross(glm::cross(ac, ao), ac);
        } else {
            return line(points = { a, b }, dir);
        }
    } else {
        if (glm::dot(glm::cross(ab, abc), ao) > 0) {
            return line(points = { a, b }, dir);
        } else {
            if (glm::dot(abc, ao) > 0) {
                dir = abc;
            } else {
                points = { a, c, b };
                dir = -abc;
            }
        }
    }
    return false;
}

bool GJK::tetrahedron(Simplex& points, glm::vec3& dir) {
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];
    glm::vec3 d = points[3];
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ad = d - a;
    glm::vec3 ao = -a;
    glm::vec3 abc = glm::cross(ab, ac);
    glm::vec3 acd = glm::cross(ac, ad);
    glm::vec3 adb = glm::cross(ad, ab);

    if (glm::dot(abc, ao) > 0) {
        return triangle(points = { a, b, c }, dir);
    } else if (glm::dot(acd, ao) > 0) {
        return triangle(points = { a, c, d }, dir);
    } else if (glm::dot(adb, ao) > 0) {
        return triangle(points = { a, d, b }, dir);
    }
    return true;
}

MTV GJK::collisionResponse(const Collider* a, const Collider* b, Simplex& simplex) {
    // Running EPA
    std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
    std::vector<size_t> faces = {0, 1, 2,
                                 0, 3, 1,
                                 0, 2, 3,
                                 1, 3, 2};
    auto result = getFaceNormals(polytope, faces);
    std::vector<glm::vec4> normals = result.first;
    size_t minFace = result.second;
    glm::vec3 minNormal;
    float minDistance = FLT_MAX;

    while (minDistance == FLT_MAX) {
        minNormal = glm::vec3(normals[minFace].x, normals[minFace].y, normals[minFace].z);
        minDistance = normals[minFace].w;

        glm::vec3 support = a->getSupportPoint(minNormal) - b->getSupportPoint(-minNormal);
        float sDistance = glm::dot(minNormal, support);
        if (std::abs(sDistance - minDistance) > 0.001f) {
            minDistance = FLT_MAX;
            std::vector<std::pair<size_t, size_t>> uniqueEdges;

            for (size_t i = 0; i < normals.size(); i++) {
                glm::vec3 temp_normal(normals[i].x, normals[i].y, normals[i].z);
                size_t f = i * 3;
                glm::vec3 facePoint = polytope[faces[f]];
                if (glm::dot(temp_normal, support - facePoint) > 0.0f) {
                    addIfUniqueEdge(uniqueEdges, faces, f, f + 1);
                    addIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                    addIfUniqueEdge(uniqueEdges, faces, f + 2, f);

                    faces[f + 2] = faces.back(); faces.pop_back();
                    faces[f + 1] = faces.back(); faces.pop_back();
                    faces[f] = faces.back(); faces.pop_back();

                    normals[i] = normals.back(); // pop-erase
                    normals.pop_back();

                    i--;
                }
            }
            std::vector<size_t> newFaces;
            for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
                newFaces.push_back(edgeIndex1);
                newFaces.push_back(edgeIndex2);
                newFaces.push_back(polytope.size());
            }
            polytope.push_back(support);
            auto result = getFaceNormals(polytope, newFaces);
            std::vector<glm::vec4> newNormals = result.first;
            size_t newMinFace = result.second;
            float oldMinDistance = FLT_MAX;
            for (size_t i = 0; i < normals.size(); i++) {
                if (normals[i].w < oldMinDistance) {
                    oldMinDistance = normals[i].w;
                    minFace = i;
                }
            }
            if (newNormals[newMinFace].w < oldMinDistance) {
                minFace = newMinFace + normals.size();
            }
            faces.insert(faces.end(), newFaces.begin(), newFaces.end());
            normals.insert(normals.end(), newNormals.begin(), newNormals.end());
        }
    }

    MTV points;
    points.normal = minNormal;
    points.depth = minDistance + 0.001f;
    points.collision = true;

    return points;
}


std::pair<std::vector<glm::vec4>, size_t> GJK::getFaceNormals(const std::vector<glm::vec3>& polytope,
                                                              const std::vector<size_t>& faces) {
    std::vector<glm::vec4> normals;
    size_t minTriangle = 0;
    float  minDistance = FLT_MAX;
    for (size_t i = 0; i < faces.size(); i += 3) {
        glm::vec3 a = polytope[faces[i]];
        glm::vec3 b = polytope[faces[i + 1]];
        glm::vec3 c = polytope[faces[i + 2]];
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        if (glm::length(normal) < 1e-10f)
            continue;
        float distance = glm::dot(normal, a);
        if (distance < 0) {
            normal   *= -1;
            distance *= -1;
        }
        normals.emplace_back(normal, distance);
        if (distance < minDistance) {
            minTriangle = i / 3;
            minDistance = distance;
        }
    }
    return { normals, minTriangle };
}

void GJK::addIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges,
                     const std::vector<size_t>& faces,
                     size_t a, size_t b) {
    auto reverse = std::find(edges.begin(),
                             edges.end(),
                             std::make_pair(faces[b], faces[a]));
    if (reverse != edges.end()) {
        edges.erase(reverse);
    } else {
        edges.emplace_back(faces[a], faces[b]);
    }
}


