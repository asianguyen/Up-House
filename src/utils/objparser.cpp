#include "objparser.h"
#include "glm/ext/vector_float3.hpp"

#include <iostream>
#include <fstream>
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

    std::unordered_map<std::string, Material> materials = parseMTL("/Users/asianguyen/Desktop/CS1230/house/untitled.mtl");

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
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF){
            break;
        } // EOF = End Of File. Quit the loop.
        // else : parse lineHeader

        if ( strcmp( lineHeader, "v" ) == 0 ){
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
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
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

            char temp_material_name[256]; // Temporary buffer for reading the material name
            fscanf(file, "%255s\n", temp_material_name); // Read into the buffer (up to 255 characters + null terminator)

            activeMaterial = temp_material_name;
        }
    }

    for( unsigned int i=0; i < vertexIndices.size(); i++ ){
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex.x);
        out_vertices.push_back(vertex.y);
        out_vertices.push_back(vertex.z);

        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_vertices[normalIndex -1 ];
        out_vertices.push_back(normal.x);
        out_vertices.push_back(normal.y);
        out_vertices.push_back(normal.z);

        Material mat = materials[materialNames[i]];


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


        // unsigned int textureIndex = uvIndices[i];
        // glm::vec3 texture = temp_vertices[textureIndex -1 ];
        // out_vertices.push_back(texture);
    }
    return true;

}
