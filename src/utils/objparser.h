#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>
class objparser
{
public:

    static bool loadOBJ(const char* path, std::vector<float> &out_vertices);
    static glm::vec3 calculateTangent(
        const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3);


};

#endif // OBJPARSER_H
