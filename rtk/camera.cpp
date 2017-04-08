//
// Created by Mehmet Fatih BAKIR on 01/04/2017.
//

#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void rtk::camera::updateCameraVectors()
{
    rotation = glm::angleAxis(glm::radians(-Pitch), glm::vec3(1,0,0));
    rotation *= glm::angleAxis(glm::radians(-Yaw), glm::vec3(0,1,0));
    rotation = glm::normalize(rotation);

    this->forward = glm::normalize(rotation * glm::vec3(0, 0, 1));
    this->Up = glm::normalize(rotation * glm::vec3(0, 1, 0));
    this->Right = glm::normalize(rotation * glm::vec3(-1, 0, 0));
}

glm::mat4 rtk::camera::GetViewMatrix() const
{
    return glm::lookAt(this->Position, this->Position + this->forward, this->Up);
}

rtk::camera::camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
        :
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVTY),
        Zoom(ZOOM)
{
    this->Position = position;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}

void rtk::camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw   += xoffset;
    this->Pitch += yoffset;

    // Update Front, Right and Up Vectors using the updated Eular angles
    this->updateCameraVectors();
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
        this->Position += this->forward * velocity;
    if (direction == BACKWARD)
        this->Position -= this->forward * velocity;
    if (direction == LEFT)
        this->Position -= this->Right * velocity;
    if (direction == RIGHT)
        this->Position += this->Right * velocity;
}

glm::mat4 rtk::camera::GetProjectionMatrix() const
{
    return glm::perspective(45.f, 800.f/600, 0.1f, 1000.f);
}
