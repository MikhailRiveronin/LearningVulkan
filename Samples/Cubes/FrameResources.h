#pragma once

#include <glm/glm.hpp>

struct ObjectResources {
    glm::mat4 world;
};

struct PassResources {
    glm::mat4 view;
    glm::mat4 proj;
};

struct FrameResources {
    Buffer objectBuffer;
    Buffer passBuffer;
};
