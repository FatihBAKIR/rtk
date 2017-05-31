//
// Created by Mehmet Fatih BAKIR on 01/04/2017.
//

#include <rtk/camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 rtk::camera::GetViewMatrix() const
{
    return glm::lookAt(this->Position, this->Position + this->get_forward(), this->get_up());
}

rtk::camera::camera(glm::vec3 position)
        :
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVTY),
        Zoom(ZOOM)
{
    this->Position = position;
}

void rtk::camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    auto yaw = glm::angleAxis(glm::radians(xoffset * MouseSensitivity), glm::vec3(0, 1, 0));
    auto pitch = glm::angleAxis(glm::radians(yoffset * MouseSensitivity), glm::vec3(1, 0, 0));

    rotation = rotation * pitch; // self
    rotation = yaw * rotation; // world

    rotation = glm::normalize(rotation);
}

void rtk::camera::ProcessMouseScroll(GLfloat yoffset)
{
    if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
        this->Zoom -= yoffset;
    if (this->Zoom <= 1.0f)
        this->Zoom = 1.0f;
    if (this->Zoom >= 45.0f)
        this->Zoom = 45.0f;
}

void rtk::camera::ProcessKeyboard(rtk::Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = this->MovementSpeed * deltaTime;
    if (direction == FORWARD)
        this->Position += get_forward() * velocity;
    if (direction == BACKWARD)
        this->Position -= get_forward() * velocity;
    if (direction == LEFT)
        this->Position += get_right() * velocity;
    if (direction == RIGHT)
        this->Position -= get_right() * velocity;
}

glm::mat4 rtk::camera::GetProjectionMatrix() const
{
    return glm::perspective(45.f, 800.f/600, 0.1f, 10000.f);
}

glm::vec3 rtk::camera::get_right() const
{
    return rotation * glm::vec3(1, 0, 0);
}

glm::vec3 rtk::camera::get_forward() const
{
    return rotation * glm::vec3(0, 0, 1);
}

glm::vec3 rtk::camera::get_up() const
{
    return rotation * glm::vec3(0, 1, 0);
}
