#include "Camera.h"
#include "KeyCodes.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(Context const& globals, glm::vec3 const& pos) :
    pos(pos),
    yaw(-glm::half_pi<float>()),
    pitch(0.f),
    mouseSensitivity(0.001f),
    movementSpeed(0.1f)
{
    recalculateViewMatrix();

    glm::mat4 X = {
        1.f , 0.f, 0.f, 0.f,
        0.f, -1.f, 0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        0.f, 0.f, 0.f, 1.f };
    X = glm::rotate(glm::mat4(1.f), glm::radians(-180.0f), glm::vec3(1, 0, 0));

    float alpha = glm::quarter_pi<float>();
    float aspect = (float)globals.swapchain.extent.width / (float)globals.swapchain.extent.height;
    float n = 0.1f;
    float f = 100.f;
    glm::mat4 proj = {
        1.f / (aspect * tan(alpha / 2.f)), 0.f, 0.f, 0.f,
        0.f, 1.f / (tan(alpha / 2.f)), 0.f, 0.f,
        0.f, 0.f, f / (f - n), 1.f,
        0.f, 0.f, -n * f / (f - n), 0.f };

    matrices.proj = proj * X;
}

void Camera::onNotify(EventType type, EventContext context)
{
    switch (type) {
    case EventType::LEFT_BUTTON_DOWN:
        lastPos.x = context.i16[0];
        lastPos.y = context.i16[1];
        return;

    case EventType::MOUSE_MOVE: {
        i16 xOffset = context.i16[0] - lastPos.x;
        i16 yOffset = context.i16[1] - lastPos.y;
        lastPos.x = context.i16[0];
        lastPos.y = context.i16[1];
        yaw -= (float)xOffset * mouseSensitivity;
        pitch += (float)yOffset * mouseSensitivity;
        pitch = std::clamp(pitch, -89.f, 89.f);

        recalculateViewMatrix();
        return;
    }

    case EventType::KEY_DOWN:
        switch (context.u16[0]) {
        case KEY_CODE_W:
            pos += target * movementSpeed;
            break;

        case KEY_CODE_S:
            pos -= target * movementSpeed;
            break;

        case KEY_CODE_A:
            pos -= right * movementSpeed;
            break;

        case KEY_CODE_D:
            pos += right * movementSpeed;
            break;

        default:
            break;
        }
        recalculateViewMatrix();
        return;

    default:
        break;
    }
}

void Camera::recalculateViewMatrix()
{
    target = glm::normalize(glm::vec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch)));
    right = glm::normalize(glm::cross(target, glm::vec3(0.f, 1.f, 0.f)));
    matrices.view = glm::lookAt(pos, pos + target, glm::vec3(0.f, 1.f, 0.f));
}
