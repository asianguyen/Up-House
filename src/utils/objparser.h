
#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <map>
#include <vector>
#include <string>

struct BoundingBox{
    glm::vec3 min;
    glm::vec3 max;
};

struct Balloon{
    int groupNumber;
    glm::vec3 pos;
    glm::vec3 velocity;
    BoundingBox bb;
    glm::mat4 modelMatrix;
};

class objparser
{
public:

    static bool loadOBJ(const char* path, std::vector<float> &out_vertices);
    static bool loadOBJBalloons(const char * path, std::vector<std::vector<float>> &out_vertices);
    static glm::vec3 calculateTangent(
        const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3);

    static std::vector<Balloon> balloons;
    static std::map<int, std::vector<size_t>> balloonVertexIndices;



};

#endif // OBJPARSER_H
