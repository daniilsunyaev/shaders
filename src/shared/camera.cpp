#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include <iostream>

Camera::Camera(glm::vec3 tPosition, glm::vec3 tUp, float tYaw, float tPitch) :
  mPosition(tPosition),
  mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mWorldUp(tUp), mYaw(tYaw), mPitch(tPitch),
  mMovementSpeed(SPEED), mMouseSensitivity(SENSITIVTY), mZoom(ZOOM) {
    updateCameraVectors();
  }

Camera::Camera(float tPosX, float tPosY, float tPosZ,
    float tUpX, float tUpY, float tUpZ,
    float tYaw, float tPitch) :
  mPosition(glm::vec3(tPosX, tPosY, tPosZ)),
  mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
  mWorldUp(glm::vec3(tUpX, tUpY, tUpZ)), mYaw(tYaw), mPitch(tPitch),
  mMovementSpeed(SPEED), mMouseSensitivity(SENSITIVTY), mZoom(ZOOM) {
    updateCameraVectors();
  }

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::processKeyboard(Camera_Movement tDirection, float tDeltaTime) {
  float velocity = mMovementSpeed * tDeltaTime;
  if (tDirection == FORWARD)
    mPosition += mFront * velocity;
  if (tDirection == BACKWARD)
    mPosition -= mFront * velocity;
  if (tDirection == LEFT)
    mPosition -= mRight * velocity;
  if (tDirection == RIGHT)
    mPosition += mRight * velocity;
}

void Camera::processMouseMovement(float tXoffset, float tYoffset,
    GLboolean tConstrainPitch) {
  tXoffset *= mMouseSensitivity;
  tYoffset *= mMouseSensitivity;

  mYaw += tXoffset;
  mPitch += tYoffset;

  if(tConstrainPitch) {
    if(mPitch > 89.0f)
      mPitch = 89.0f;
    if(mPitch < -89.0f)
      mPitch = -89.0f;
  }
  updateCameraVectors();
}

void Camera::processMouseScroll(float tYoffset) {
  if(mZoom >= 1.0f && mZoom <= 45.0f)
    mZoom -= tYoffset;
  if(mZoom <= 1.0f)
    mZoom = 1.0f;
  if(mZoom >= 45.0f)
    mZoom = 45.0f;
}

void Camera::updateCameraVectors() {
  glm::dvec3 front;
  front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
  front.y = sin(glm::radians(mPitch));
  front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
  mFront = glm::normalize(front);
  mRight = glm::normalize(glm::cross(mFront, mWorldUp));
  mUp = glm::normalize(glm::cross(mRight, mFront));
}

const glm::vec3& Camera::getPosition() const {
  return mPosition;
}

const float Camera::getYaw() const {
  return mYaw;
}

const float Camera::getPitch() const {
  return mPitch;
}

const float Camera::getZoom() const {
  return mZoom;
}
