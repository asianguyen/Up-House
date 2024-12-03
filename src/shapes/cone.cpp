#include "cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = glm::max(3, param2);
    setVertexData();
}

void Cone::makeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {
    // Calculate the base normals for the bottom vertices
    glm::vec3 normalBottomLeft = glm::normalize(glm::vec3(
        bottomLeft.x,
        0.5f * sqrt(bottomLeft.x * bottomLeft.x + bottomLeft.z * bottomLeft.z),
        bottomLeft.z
        ));
    glm::vec3 normalBottomRight = glm::normalize(glm::vec3(
        bottomRight.x,
        0.5f * sqrt(bottomRight.x * bottomRight.x + bottomRight.z * bottomRight.z),
        bottomRight.z
        ));

    glm::vec3 averageNormal = glm::normalize(0.5f * (normalBottomLeft + normalBottomRight));
    averageNormal.y = 0;
    averageNormal = glm::normalize(averageNormal);
    averageNormal.y = 0.5f;
    averageNormal = glm::normalize(averageNormal);

    //need a small threshold for floating point comparisons
    const float EPSILON = 1e-6f;
    bool isTopLeftTip = (abs(topLeft.x) < EPSILON && abs(topLeft.z) < EPSILON);
    bool isTopRightTip = (abs(topRight.x) < EPSILON && abs(topRight.z) < EPSILON);

    glm::vec3 normalTopLeft, normalTopRight;

    if (isTopLeftTip) {
        normalTopLeft = averageNormal;
    } else {
        normalTopLeft = glm::normalize(glm::vec3(
            topLeft.x,
            0.5f * sqrt(topLeft.x * topLeft.x + topLeft.z * topLeft.z),
            topLeft.z
            ));
    }

    if (isTopRightTip) {
        normalTopRight = averageNormal;
    } else {
        normalTopRight = glm::normalize(glm::vec3(
            topRight.x,
            0.5f * sqrt(topRight.x * topRight.x + topRight.z * topRight.z),
            topRight.z
            ));
    }

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

}

void Cone::makeSide() {
    float radius = 0.5f;
    float thetaStep = glm::radians(360.f / m_param2);
    float heightStep = 1.0f / m_param1;

    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        for (int j = 0; j < m_param1; j++) {
            float yBottom = -0.5f + j * heightStep;
            float yTop = -0.5f + (j + 1) * heightStep;

            float bottomRadius = radius * (1.0f - (float)j / m_param1);
            float topRadius = radius * (1.0f - (float)(j + 1) / m_param1);

            glm::vec3 bottomLeft(bottomRadius * cos(currentTheta), yBottom, bottomRadius * sin(currentTheta));
            glm::vec3 bottomRight(bottomRadius * cos(nextTheta), yBottom, bottomRadius * sin(nextTheta));
            glm::vec3 topLeft(topRadius * cos(currentTheta), yTop, topRadius * sin(currentTheta));
            glm::vec3 topRight(topRadius * cos(nextTheta), yTop, topRadius * sin(nextTheta));

            makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
        }
    }
}

void Cone::makeBase() {
    float radius = 0.5f;
    float thetaStep = glm::radians(360.f / m_param2);
    glm::vec3 center(0.0f, -0.5f, 0.0f);
    glm::vec3 normal(0.0f, -1.0f, 0.0f);

    for (int j = 0; j < m_param1; ++j) {
        float currentRadius = (radius / m_param1) * j;
        float nextRadius = (radius / m_param1) * (j + 1);

        for (int i = 0; i < m_param2; ++i) {
            float currentTheta = i * thetaStep;
            float nextTheta = (i + 1) * thetaStep;

            glm::vec3 edge1(currentRadius * cos(currentTheta), -0.5f, currentRadius * sin(currentTheta));
            glm::vec3 edge2(currentRadius * cos(nextTheta), -0.5f, currentRadius * sin(nextTheta));

            glm::vec3 outerEdge1(nextRadius * cos(currentTheta), -0.5f, nextRadius * sin(currentTheta));
            glm::vec3 outerEdge2(nextRadius * cos(nextTheta), -0.5f, nextRadius * sin(nextTheta));

            insertVec3(m_vertexData, edge1);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, outerEdge1);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, outerEdge2);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, edge1);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, outerEdge2);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, edge2);
            insertVec3(m_vertexData, normal);
        }
    }
}


void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeSide();
    makeBase();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

std::vector<float> Cone::generateShape() {
    return m_vertexData;
}
