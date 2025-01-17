

#version 330 core

in vec3 texCoords;

uniform samplerCube skybox;

out vec4 fragColor;

void main() {
    fragColor = texture(skybox, texCoords);
}
