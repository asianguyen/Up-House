// attribute vec4 qt_Vertex;
// attribute vec4 qt_MultiTexCoord0;
// uniform mat4 qt_ModelViewProjectionMatrix;
// varying vec4 qt_TexCoord0;

// void main(void)
// {
//     gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
//     qt_TexCoord0 = qt_MultiTexCoord0;
// }


// #version 330 core
// layout (location = 0) in vec3 aPos;

// out vec3 TexCoords;

// uniform mat4 projection;
// uniform mat4 view;

// void main()
// {
//     TexCoords = aPos;
//     gl_Position = projection * view * vec4(aPos, 1.0);
// }

#version 330 core
layout(location = 0) in vec3 position;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0);
    texCoords = position;
}
