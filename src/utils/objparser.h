#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>
class objparser
{
public:

    static bool loadOBJ(const char* path, std::vector<float> &out_vertices);

};

#endif // OBJPARSER_H
