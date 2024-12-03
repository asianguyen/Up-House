#include "camera.h"
#include <stdexcept>
#include <math.h>
#include <iostream>

glm::mat4 Camera::getProjectionMatrix(float aspect, float thetaH, float nearPlane, float farPlane) const {
    float tanHalfThetaH = tan(thetaH / 2.0f);
    float tanHalfThetaW = tanHalfThetaH * aspect;
    float c = -nearPlane/farPlane;

    glm::mat4 scaleMatrix = glm::mat4(0.0f);
    scaleMatrix[0][0] = 1.0f / (farPlane * tanHalfThetaW);
    scaleMatrix[1][1] = 1.0f / (farPlane * tanHalfThetaH);
    scaleMatrix[2][2] = 1.0f / farPlane;
    scaleMatrix[3][3] = 1.0f;

    glm::mat4 unhingingMatrix = glm::mat4(0.0f);
    unhingingMatrix[0][0] = 1.0f;
    unhingingMatrix[1][1] = 1.0f;
    unhingingMatrix[2][2] = 1.0f / (1.0f + c);
    unhingingMatrix[3][2] = -c / (1.0f + c);
    unhingingMatrix[2][3] = -1.0f;

    glm::mat4 perspectiveMatrix = glm::mat4(0.0f);
    perspectiveMatrix[0][0] = 1.0f;
    perspectiveMatrix[1][1] = 1.0f;
    perspectiveMatrix[2][2] = -2.0f;
    perspectiveMatrix[3][2] = -1.0f;
    perspectiveMatrix[3][3] = 1.0f;

    //combine matrices
    glm::mat4 projectionMatrix = perspectiveMatrix * unhingingMatrix * scaleMatrix;

    return projectionMatrix;
}


//from lab 4
glm::mat4 Camera::getViewMatrix(glm::vec3 pos, glm::vec3 look, glm::vec3 up) const {

    glm::vec3 w = glm::normalize(-look);

    glm::vec3 v = glm::normalize(up - glm::dot(up, w)*w);

    glm::vec3 u = glm::cross(v, w);

    //rotation matrix
    glm::mat4 rotation = glm::mat4(
        u.x, v.x, w.x, 0.0f,
        u.y, v.y, w.y, 0.0f,
        u.z, v.z, w.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    glm::mat4 translation = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -pos.x, -pos.y, -pos.z, 1.0f
        );

    return rotation * translation;
}
float Camera::getAspectRatio() const {
    throw std::runtime_error("not implemented");
}

float Camera::getHeightAngle(SceneCameraData cameraData) const {
    return cameraData.heightAngle;
}

float Camera::getFocalLength(SceneCameraData cameraData) const {
    //throw std::runtime_error("not implemented");
    return cameraData.focalLength;
}

float Camera::getAperture(SceneCameraData cameraData) const {
    //throw std::runtime_error("not implemented");
    return cameraData.aperture;
}
