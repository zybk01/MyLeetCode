#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec4 TexCoordtemp;
uniform vec4 inputD;
uniform vec4 inputR;
uniform mat4 transform;
uniform mat4 perspective;
uniform mat4 view;

void main()
{
    gl_Position =  perspective * view* transform * vec4(aPos, 1.0);
    ourColor = vec3(0.5, 0.5 ,0.5);
    TexCoord = aTexCoord;
}