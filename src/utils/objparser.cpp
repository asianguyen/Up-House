// #include "objparser.h"
// #include "glm/ext/vector_float3.hpp"

// objparser::objparser() {


// }

// bool loadOBJ(const char * path, std::vector < glm::vec3 > & out_vertices,
//              std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals){

//     std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
//     std::vector< glm::vec3 > temp_vertices;
//     std::vector< glm::vec2 > temp_uvs;
//     std::vector< glm::vec3 > temp_normals;

//     FILE * file = fopen(path, "r");
//     if( file == NULL ){
//         printf("Impossible to open the file !\n");
//         return false;
//     }

//     while(1){

//         char lineHeader[256];
//         // read the first word of the line
//         int res = fscanf(file, "%s", lineHeader);
//         if (res == EOF){
//             break; } // EOF = End Of File. Quit the loop.
//         el
//         // else : parse lineHeader


// };

// }else if ( strcmp( lineHeader, "f" ) == 0 ){
//     std::string vertex1, vertex2, vertex3;
//     unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
//     int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
//     if (matches != 9){
//         printf("File can't be read by our simple parser : ( Try exporting with other options\n");
//         return false;
//     }
//     vertexIndices.push_back(vertexIndex[0]);
//     vertexIndices.push_back(vertexIndex[1]);
//     vertexIndices.push_back(vertexIndex[2]);
//     uvIndices    .push_back(uvIndex[0]);
//     uvIndices    .push_back(uvIndex[1]);
//     uvIndices    .push_back(uvIndex[2]);
//     normalIndices.push_back(normalIndex[0]);
//     normalIndices.push_back(normalIndex[1]);
//     normalIndices.push_back(normalIndex[2]);
