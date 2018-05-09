#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

const float YAW        = -90.0f;
const float PITCH      =  0.0f;
const float SPEED      =  2.5f;
const float SENSITIVTY =  0.3f;
const float ZOOM       =  45.0f;

class Camera {
  public:
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mWorldUp;
    float mYaw;
    float mPitch;
    float mMovementSpeed;
    float mMouseSensitivity;
    float mZoom;
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch);
    glm::mat4 GetViewMatrix();
    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset,
        GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);
  private:
    void updateCameraVectors();
};
