//
// Created by Mehmet Fatih BAKIR on 01/04/2017.
//

// ripoff from learnopengl

#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>

namespace rtk
{
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    const GLfloat YAW        =  0.0f;
    const GLfloat PITCH      =  0.0f;
    const GLfloat SPEED      =  15.0f;
    const GLfloat SENSITIVTY =  0.25f;
    const GLfloat ZOOM       =  45.0f;

    class camera
    {
        glm::vec3 Position;
        glm::vec3 forward;
        glm::vec3 Up;
        glm::vec3 Right;

        glm::quat rotation;

        GLfloat Yaw;
        GLfloat Pitch;

        GLfloat MovementSpeed;
        GLfloat MouseSensitivity;
        GLfloat Zoom;
    public:
        camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                GLfloat yaw = YAW,
                GLfloat pitch = PITCH);

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;

        // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(GLfloat yoffset);

        glm::vec3 get_position() const {
            return Position;
        }

    private:
        // Calculates the front vector from the Camera's (updated) Eular Angles
        void updateCameraVectors();
    };
}