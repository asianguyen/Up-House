#pragma once

#include <glm/glm.hpp>
#include "src/utils/scenedata.h"

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    // Constructor accepting SceneCameraData
    Camera(const SceneCameraData& cameraData)
        : position(cameraData.pos),
        direction(cameraData.look),
        upVector(cameraData.up),
        heightAngle(cameraData.heightAngle),
        aperture(cameraData.aperture),
        focalLength(cameraData.focalLength){}

    glm::mat4 getProjectionMatrix(float aspect, float thetaH, float nearPlane, float farPlane) const;

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix(glm::vec3 pos, glm::vec3 look, glm::vec3 up) const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle(SceneCameraData cameraData) const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength(SceneCameraData cameraData) const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture(SceneCameraData cameraData) const;

private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 upVector;
    float heightAngle;
    float aperture;
    float focalLength;
};
