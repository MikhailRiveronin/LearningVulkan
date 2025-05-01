#pragma once

#include "Defines.h"
#include "EventManager.h"
#include "Structures.h"

#include <glm/glm.hpp>

class Camera : public Listener {
public:
    struct {
        glm::mat4 view = glm::mat4(1.f);
        glm::mat4 proj = glm::mat4(1.f);
    } matrices;

    glm::vec3 pos;

    Camera(Globals const& globals, glm::vec3 const& pos);

    void onNotify(EventType type, EventContext context) override;

private:
    glm::vec3 target;
    glm::vec3 right;
    float yaw;
    float pitch;

    struct {
        i16 x;
        i16 y;
    } lastPos;

    float mouseSensitivity;
    float movementSpeed;

    void recalculateViewMatrix();
};
