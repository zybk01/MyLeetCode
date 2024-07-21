#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 inputD;
uniform vec4 inputR;
uniform mat4 transform;
uniform vec4 textureDimen;

void main()
{
    // FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0 * inputD.x);
    vec4 texture1 = texture(ourTexture, TexCoord);
    vec4 texture2 = texture(ourTexture, TexCoord + vec2(0.002 , 0));
    //FragColor = texture(ourTexture, TexCoord);
    // FragColor = vec4( texture1.x, texture1.y, texture1.z, texture1.a);
    FragColor = vec4( texture2.x, texture1.y, texture1.z, texture2.a);
}