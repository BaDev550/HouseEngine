#pragma once

#include <iostream>
#include "Renderer/Model.h"
#include <glm/glm.hpp>

// From learnopengl.com
struct Frustum {
    glm::vec4 Planes[6];

    void Update(const glm::mat4& viewProj) {
        Planes[0] = viewProj[3] + viewProj[0];
        Planes[1] = viewProj[3] - viewProj[0];
        Planes[2] = viewProj[3] + viewProj[1];
        Planes[3] = viewProj[3] - viewProj[1];
        Planes[4] = viewProj[3] + viewProj[2];
        Planes[5] = viewProj[3] - viewProj[2];

        for (int i = 0; i < 6; i++) {
            float length = glm::length(glm::vec3(Planes[i]));
            Planes[i] /= length;
        }
    }

    bool IsInside(const glm::vec3& center, float radius) const {
        for (int i = 0; i < 6; i++) {
            if (glm::dot(glm::vec3(Planes[i]), center) + Planes[i].w < -radius)
                return false;
        }
        return true;
    }

    bool IsBoxInFrustum(const House::AABB& box, const glm::mat4& transform) {
        glm::vec3 center = glm::vec3(transform * glm::vec4(box.GetCenter(), 1.0f));

        glm::vec3 extents = box.GetExtents();
        glm::mat3 absMatrix = glm::mat3(transform);
        glm::vec3 worldExtents = absMatrix * extents;

        for (int i = 0; i < 6; i++) {
            float projection = glm::dot(worldExtents, glm::abs(glm::vec3(Planes[i])));
            float distance = glm::dot(glm::vec3(Planes[i]), center) + Planes[i].w;

            if (distance < -projection)
                return false;
        }
        return true;
    }
};