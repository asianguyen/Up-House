#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = glm::max(2, param1);
    m_param2 = glm::max(3, param2);
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!

    glm::vec3 normalTopLeft = glm::normalize(topLeft);
    glm::vec3 normalTopRight = glm::normalize(topRight);
    glm::vec3 normalBottomLeft = glm::normalize(bottomLeft);
    glm::vec3 normalBottomRight = glm::normalize(bottomRight);

    //triangle 1
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    //triangle 2
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);

}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    float phiStep = glm::radians(180.f/m_param1);
    float radius = 0.5f;

    // Start from the top (0) and go to PI
    for (int i = 0; i < m_param1; i++) {
        float nextPhi = i * phiStep;
        float currentPhi = (i + 1) * phiStep;

        glm::vec3 topLeft(radius * glm::sin(currentPhi) * glm::cos(currentTheta),
                          radius * glm::cos(currentPhi),
                          radius * glm::sin(currentPhi) * glm::sin(currentTheta));

        glm::vec3 topRight(radius * glm::sin(currentPhi) * glm::cos(nextTheta),
                           radius * glm::cos(currentPhi),
                           radius * glm::sin(currentPhi) * glm::sin(nextTheta));

        glm::vec3 bottomLeft(radius * glm::sin(nextPhi) * glm::cos(currentTheta),
                             radius * glm::cos(nextPhi),
                             radius * glm::sin(nextPhi) * glm::sin(currentTheta));

        glm::vec3 bottomRight(radius * glm::sin(nextPhi) * glm::cos(nextTheta),
                              radius * glm::cos(nextPhi),
                              radius * glm::sin(nextPhi) * glm::sin(nextTheta));

        // glm::vec3 topLeft(radius * glm::sin(currentPhi) * glm::sin(currentTheta),
        //                   radius * glm::cos(currentPhi),
        //                   radius * glm::sin(currentPhi) * glm::cos(currentTheta)
        //                   );

        // glm::vec3 topRight(radius * glm::sin(currentPhi) * glm::sin(nextTheta),
        //                    radius * glm::cos(currentPhi),
        //                    radius * glm::sin(currentPhi) * glm::cos(nextTheta)
        //                    );

        // glm::vec3 bottomLeft(radius * glm::sin(nextPhi) * glm::sin(currentTheta),
        //                      radius * glm::cos(nextPhi),
        //                      radius * glm::sin(nextPhi) * glm::cos(currentTheta));

        // glm::vec3 bottomRight(radius * glm::sin(nextPhi) * glm::sin(nextTheta),
        //                       radius * glm::cos(nextPhi),
        //                       radius * glm::sin(nextPhi) * glm::cos(nextTheta));

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!

    float thetaStep = glm::radians(360.f/m_param2);

    for (int j = 0; j < m_param2; j++) {
        float currentTheta = j * thetaStep;
        float nextTheta = (j + 1) * thetaStep;

        makeWedge(currentTheta, nextTheta);
    }

}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

    // float thetaStep = glm::radians(360.f / m_param2);
    // float currentTheta = 0 * thetaStep;
    // float nextTheta = 1 * thetaStep;
    // makeWedge(currentTheta, nextTheta);

    // Uncomment these lines to make sphere for Task 7:

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

std::vector<float> Sphere::generateShape() {
    return m_vertexData;
}
