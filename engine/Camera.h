#pragma once

#include "defines.h"

#include "EventManager.h"
#include "vulkan_classes.h"
#include "movable.h"
#include "object.h"



class Camera : public Listener, public Object, public Movable
{
public:
    struct {
        glm::mat4 view = glm::mat4(1.f);
        glm::mat4 proj = glm::mat4(1.f);
    } matrices;

    std::string name;
    float aspect_ratio;
    float y_fov;
    float z_near;
    float z_far;
    // glm::vec3 target;

    Camera();

    void onNotify(EventType type, EventContext context) override;

private:
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
