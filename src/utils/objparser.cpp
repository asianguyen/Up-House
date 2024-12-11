#include "objparser.h"
#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Structure to store material properties
struct Material {
    std::vector<float> Ka; // Ambient color (r, g, b)
    std::vector<float> Kd; // Diffuse color (r, g, b)
    std::vector<float> Ks; // Specular color (r, g, b)
    float Ns = 0.0f;       // Specular exponent
    float d = 1.0f;        // Transparency (default to opaque)
    std::string map_Kd;    // Diffuse texture map
};

std::vector<Balloon> objparser::balloons;
std::map<int, std::vector<size_t>> objparser::balloonVertexIndices;

// Function to parse an .mtl file
std::unordered_map<std::string, Material> parseMTL(const std::string& filepath) {
    std::unordered_map<std::string, Material> materials;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the .mtl file: " << filepath << std::endl;
        return materials;
    }

    std::string line;
    Material currentMaterial;
    std::string currentMaterialName;

    while (std::getline(file, line)) {
        // Ignore comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream lineStream(line);
        std::string keyword;
        lineStream >> keyword;

        if (keyword == "newmtl") {
            if (!currentMaterialName.empty()) {
                // Save the previous material
                materials[currentMaterialName] = currentMaterial;
            }
            // Start a new material
            lineStream >> currentMaterialName;
            currentMaterial = Material(); // Reset material properties
        } else if (keyword == "Ka") {
            float r, g, b;
            lineStream >> r >> g >> b;
            currentMaterial.Ka = {r, g, b};
        } else if (keyword == "Kd") {
            float r, g, b;
            lineStream >> r >> g >> b;
            currentMaterial.Kd = {r, g, b};
        } else if (keyword == "Ks") {
            float r, g, b;
            lineStream >> r >> g >> b;
            currentMaterial.Ks = {r, g, b};
        } else if (keyword == "Ns") {
            lineStream >> currentMaterial.Ns;
        } else if (keyword == "d" || keyword == "Tr") {
            // Transparency can be specified as "d" or "Tr"
            lineStream >> currentMaterial.d;
            if (keyword == "Tr") {
                currentMaterial.d = 1.0f - currentMaterial.d; // Convert to "d" convention
            }
        } else if (keyword == "map_Kd") {
            lineStream >> currentMaterial.map_Kd;
        }
    }

    // Save the last material
    if (!currentMaterialName.empty()) {
        materials[currentMaterialName] = currentMaterial;
    }

    file.close();
    return materials;
}



bool objparser::loadOBJ(const char * path, std::vector<float> & out_vertices){

    std::string mtlPath = path;
    size_t dotPosition = mtlPath.find_last_of('.');
    if (dotPosition != std::string::npos) {
        mtlPath.replace(dotPosition, std::string::npos, ".mtl");
    }

    std::unordered_map<std::string, Material> materials = parseMTL(mtlPath);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<std::string> materialNames;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::string activeMaterial;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while(1){
        char lineHeader[256];

        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF){
            std::cout << "EOF " <<std::endl;
            break;
        }

        if ( strcmp( lineHeader, "v" ) == 0){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);


        } else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);

        } else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);

        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);

            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);

            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);

            materialNames.push_back(activeMaterial);
            materialNames.push_back(activeMaterial);
            materialNames.push_back(activeMaterial);
        } else if (strcmp( lineHeader, "usemtl" ) == 0 ) {
            char temp_material_name[256];
            fscanf(file, "%255s\n", temp_material_name);
            activeMaterial = temp_material_name;
        }
    }

    // Now that we have the vertex, uv, normal indices, and materials, calculate tangents
    for( unsigned int i = 0; i < vertexIndices.size(); i += 3 ){
        // Get the three vertices and uvs for each triangle
        glm::vec3 v1 = temp_vertices[vertexIndices[i] - 1];
        glm::vec3 v2 = temp_vertices[vertexIndices[i+1] - 1];
        glm::vec3 v3 = temp_vertices[vertexIndices[i+2] - 1];
        glm::vec2 uv1 = temp_uvs[uvIndices[i] - 1];
        glm::vec2 uv2 = temp_uvs[uvIndices[i+1] - 1];
        glm::vec2 uv3 = temp_uvs[uvIndices[i+2] - 1];

        // Calculate the tangent for this triangle
        glm::vec3 tangent = calculateTangent(v1, v2, v3, uv1, uv2, uv3);

        // Now add the vertices, normals, UVs, material properties, and tangent
        for (unsigned int j = 0; j < 3; j++) {
            unsigned int vertexIndex = vertexIndices[i + j];
            glm::vec3 vertex = temp_vertices[vertexIndex - 1];
            out_vertices.push_back(vertex.x);
            out_vertices.push_back(vertex.y);
            out_vertices.push_back(vertex.z);

            unsigned int normalIndex = normalIndices[i + j];
            glm::vec3 normal = temp_normals[normalIndex - 1];
            out_vertices.push_back(normal.x);
            out_vertices.push_back(normal.y);
            out_vertices.push_back(normal.z);

            //material
            Material mat = materials[materialNames[i + j]];
            out_vertices.push_back(mat.Ka[0]);
            out_vertices.push_back(mat.Ka[1]);
            out_vertices.push_back(mat.Ka[2]);

            out_vertices.push_back(mat.Kd[0]);
            out_vertices.push_back(mat.Kd[1]);
            out_vertices.push_back(mat.Kd[2]);

            out_vertices.push_back(mat.Ks[0]);
            out_vertices.push_back(mat.Ks[1]);
            out_vertices.push_back(mat.Ks[2]);

            out_vertices.push_back(mat.Ns);


            // UV coordinates
            unsigned int uvIndex = uvIndices[i + j];
            glm::vec2 uv = temp_uvs[uvIndex - 1];
            out_vertices.push_back(uv.x);
            out_vertices.push_back(uv.y);

            //tangent
            out_vertices.push_back(tangent.x);
            out_vertices.push_back(tangent.y);
            out_vertices.push_back(tangent.z);
        }
    }
    return true;
}

bool objparser::loadOBJBalloons(const char * path, std::vector<std::vector<float>> &out_vertices){

    std::string mtlPath = path;
    size_t dotPosition = mtlPath.find_last_of('.');
    if (dotPosition != std::string::npos) {
        mtlPath.replace(dotPosition, std::string::npos, ".mtl");
    }

    std::unordered_map<std::string, Material> materials = parseMTL(mtlPath);

    std::vector<int> vertexIndices, uvIndices, normalIndices, balloonIndices;
    std::vector<std::string> materialNames;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::string activeMaterial;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }
    //std::map<std::string, BoundingBox> balloonBoundingBoxes;
    int groupNum = 0;

    while(1){
        char lineHeader[256];

        int res = fscanf(file, "%s", lineHeader);

        if (res == EOF){
            std::cout << "EOF Balloons" <<std::endl;
            break;
        }



        if (strcmp(lineHeader, "o") == 0){
            char objType[256];
            fscanf(file, "%s", objType);
            if (strncmp(objType, "Sphere", 6) == 0) {
                groupNum++;
            }
            continue;
        }


        if ( strcmp( lineHeader, "v" ) == 0){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);


        } else if ( strcmp( lineHeader, "vt" ) == 0 ){

            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);


        } else if (strcmp( lineHeader, "vn" ) == 0 ){

            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);

        } else if (strcmp( lineHeader, "f" ) == 0 ){

            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9){
                std::cout << "Expected 9 matches, but got: " << matches << std::endl;

                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);

            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);

            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);

            materialNames.push_back(activeMaterial);
            materialNames.push_back(activeMaterial);
            materialNames.push_back(activeMaterial);

            balloonIndices.push_back(groupNum);
            balloonIndices.push_back(groupNum);
            balloonIndices.push_back(groupNum);
        }
        else if (strncmp(lineHeader, "usemtl", 6) == 0 ) {

            char temp_material_name[256];
            fscanf(file, "%255s\n", temp_material_name);
            activeMaterial = temp_material_name;
        }
    }

    int balloonNumCurrent = 0;
    std::vector<float> balloonVerticesFinal;
    // Now that we have the vertex, uv, normal indices, and materials, calculate tangents
    for( unsigned int i = 0; i < vertexIndices.size(); i += 3 ){
        // Get the three vertices and uvs for each triangle
        glm::vec3 v1 = temp_vertices[vertexIndices[i] - 1];
        glm::vec3 v2 = temp_vertices[vertexIndices[i+1] - 1];
        glm::vec3 v3 = temp_vertices[vertexIndices[i+2] - 1];
        glm::vec2 uv1 = temp_uvs[uvIndices[i] - 1];
        glm::vec2 uv2 = temp_uvs[uvIndices[i+1] - 1];
        glm::vec2 uv3 = temp_uvs[uvIndices[i+2] - 1];

        int balloonNum = balloonIndices[i];
        if (balloonNum != balloonNumCurrent) {
            out_vertices.push_back(balloonVerticesFinal);
            balloonVerticesFinal.clear();
            balloonNumCurrent = balloonNum;
        }

        // Calculate the tangent for this triangle
        glm::vec3 tangent = calculateTangent(v1, v2, v3, uv1, uv2, uv3);

        // Now add the vertices, normals, UVs, material properties, and tangent
        for (unsigned int j = 0; j < 3; j++) {
            unsigned int vertexIndex = vertexIndices[i + j];
            glm::vec3 vertex = temp_vertices[vertexIndex - 1];
            balloonVerticesFinal.push_back(vertex.x);
            balloonVerticesFinal.push_back(vertex.y);
            balloonVerticesFinal.push_back(vertex.z);

            unsigned int normalIndex = normalIndices[i + j];
            glm::vec3 normal = temp_normals[normalIndex - 1];
            balloonVerticesFinal.push_back(normal.x);
            balloonVerticesFinal.push_back(normal.y);
            balloonVerticesFinal.push_back(normal.z);

            //material
            Material mat = materials[materialNames[i + j]];
            balloonVerticesFinal.push_back(mat.Ka[0]);
            balloonVerticesFinal.push_back(mat.Ka[1]);
            balloonVerticesFinal.push_back(mat.Ka[2]);

            balloonVerticesFinal.push_back(mat.Kd[0]);
            balloonVerticesFinal.push_back(mat.Kd[1]);
            balloonVerticesFinal.push_back(mat.Kd[2]);

            balloonVerticesFinal.push_back(mat.Ks[0]);
            balloonVerticesFinal.push_back(mat.Ks[1]);
            balloonVerticesFinal.push_back(mat.Ks[2]);

            balloonVerticesFinal.push_back(mat.Ns);


            // UV coordinates
            unsigned int uvIndex = uvIndices[i + j];
            glm::vec2 uv = temp_uvs[uvIndex - 1];
            balloonVerticesFinal.push_back(uv.x);
            balloonVerticesFinal.push_back(uv.y);

            //tangent
            balloonVerticesFinal.push_back(tangent.x);
            balloonVerticesFinal.push_back(tangent.y);
            balloonVerticesFinal.push_back(tangent.z);
        }
    }
    return true;
}


//calculation of tangents for bump mapping
glm::vec3 objparser::calculateTangent(
    const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
    const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
{
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


    return glm::normalize(tangent);
}
