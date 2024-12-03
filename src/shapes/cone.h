#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "shape.h"

class Cone: public Shape
{
public:
    void updateParams(int param1, int param2 = 0) override;
    std::vector<float> generateShape() override;

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight);
    void makeSide();
    void makeBase();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
