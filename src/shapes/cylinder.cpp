#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = glm::max(3, param2);
    setVertexData();
}

void Cylinder::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 edge1 = topRight - topLeft;
    glm::vec3 edge2 = bottomLeft - topLeft;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    //triangle 1
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalize(glm::vec3(topLeft.x, 0, topLeft.z)));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z)));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalize(glm::vec3(bottomRight.x, 0, bottomRight.z)));

    //triangle 2

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalize(glm::vec3(topLeft.x, 0, topLeft.z)));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalize(glm::vec3(bottomRight.x, 0, bottomRight.z)));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalize(glm::vec3(topRight.x, 0, topRight.z)));




}

void Cylinder::makeCapTile(glm::vec3 center, glm::vec3 edge1, glm::vec3 edge2, bool isTop) {
    glm::vec3 normal;
    if (isTop) {
        normal = glm::normalize(glm::cross(edge1 - center, edge2 - center));

    } else {
        normal = glm::normalize(glm::cross(edge2 - center, edge1 - center));
    }

    insertVec3(m_vertexData, center);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, isTop ? edge1 : edge2);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, isTop ? edge2 : edge1);
    insertVec3(m_vertexData, normal);
}

void Cylinder::makeSide() {
    float radius = 0.5f;
    float thetaStep = glm::radians(360.f/m_param2);
    float heightStep = 1.0f / m_param1;

    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        for (int j = 0; j < m_param1; j++) {
            float yTop = - 0.5f + j * heightStep;
            float yBottom = -0.5f + (j+1) * heightStep;

            glm::vec3 topLeft(radius * cos(currentTheta), yTop, radius * sin(currentTheta));
            glm::vec3 topRight(radius * cos(nextTheta), yTop, radius * sin(nextTheta));
            glm::vec3 bottomLeft(radius * cos(currentTheta), yBottom, radius * sin(currentTheta));
            glm::vec3 bottomRight(radius * cos(nextTheta), yBottom, radius * sin(nextTheta));

            makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
        }
    }
}


void Cylinder::makeCap(float y, bool isTop) {
    float totalRadius = 0.5f;
    float thetaStep = glm::radians(360.f / m_param2);
    glm::vec3 center(0.0f, y, 0.0f);

    for (int j = 0; j < m_param1; ++j) {
        float currentRadius = totalRadius * (j + 1) / m_param1;
        float nextRadius = totalRadius * j / m_param1;

        for (int i = 0; i < m_param2; i++) {
            float currentTheta = i * thetaStep;
            float nextTheta = (i + 1) * thetaStep;

            glm::vec3 outerEdge1(currentRadius * cos(currentTheta), y, currentRadius * sin(currentTheta));
            glm::vec3 outerEdge2(currentRadius * cos(nextTheta), y, currentRadius * sin(nextTheta));
            glm::vec3 innerEdge1(nextRadius * cos(currentTheta), y, nextRadius * sin(currentTheta));
            glm::vec3 innerEdge2(nextRadius * cos(nextTheta), y, nextRadius * sin(nextTheta));

            //create two triangles per segment to form a quad between rings
            makeCapTile(outerEdge1, innerEdge1, innerEdge2, isTop);
            makeCapTile(outerEdge1, innerEdge2, outerEdge2, isTop);
        }
    }
}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeSide();
    makeCap(0.5f, true);  //Top cap
    makeCap(-0.5f, false); //Bottom cap

}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

std::vector<float> Cylinder::generateShape() {
    return m_vertexData;
}
