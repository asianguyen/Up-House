#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "shape.h"

class Cylinder: public Shape
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
    void makeCapTile(glm::vec3 center, glm::vec3 edge1, glm::vec3 edge2, bool isTop);
    void makeSide();
    void makeCap(float y, bool isTop);


    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
