#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>
class objparser
{
public:
    objparser();
    bool load_obj(const char* path, std::vector <glm::vec3> &out_vertices, std::vector <glm::vec2> &out_uvs,
                  std::vector <glm::vec3> &out_normals);
};

#endif // OBJPARSER_H
