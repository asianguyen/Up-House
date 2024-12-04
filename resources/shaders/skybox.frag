// uniform sampler2D qt_Texture0;
// varying vec4 qt_TexCoord0;

// void main(void)
// {
//     gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
// }

#version 330 core

in vec3 texCoords;

uniform samplerCube skybox;

out vec4 fragColor;

void main() {
    fragColor = texture(skybox, texCoords);
}
