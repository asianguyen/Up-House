#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 ka;
layout(location = 3) in vec3 kd;
layout(location = 4) in vec3 ks;

layout(location = 5) in vec2 textureCoord;
layout(location = 6) in vec3 tangent;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 mat_ka;
out vec3 mat_kd;
out vec3 mat_ks;

out vec2 TextureCoords;
out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {

    worldPosition = vec3(modelMatrix * vec4(position, 1.0));

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    worldNormal = normalize(normalMatrix * normal);

    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = worldNormal;
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    TextureCoords = textureCoord;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

    mat_ka = ka;
    mat_kd = kd;
    mat_ks = ks;
    mat_shininess = shininess;

}
