#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <glm/glm.hpp>

class Shape
{
public:
    virtual void updateParams(int param1, int param2 = 0) = 0;
    virtual std::vector<float> generateShape() = 0;

    virtual ~Shape() = default;
};

#endif // SHAPE_H
