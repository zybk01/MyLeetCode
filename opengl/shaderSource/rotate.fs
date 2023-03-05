#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 inputD;
uniform vec4 inputR;
uniform mat4 transform;

void main()
{
    // FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0 * inputD.x);
    FragColor = texture(ourTexture, TexCoord);
}