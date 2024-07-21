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
    float stepW = 1 / textureDimen.x;
    float stepH = 1 / textureDimen.y;
    vec4 texture1;
    float range = inputD.x * 100;
    if (range < 1) range = 1;
    float square = (range * 2 -1) * (range * 2 -1);
    //  = texture(ourTexture, TexCoord);
    for (int i = 0; i < range; i++) {
        for (int j = 0; j < range; j++) {
            texture1 = texture1 + texture(ourTexture, TexCoord + vec2(stepW * j , stepH * i)) / square;
            if (i > 0 && j > 0) {
                texture1 = texture1 + texture(ourTexture, TexCoord + vec2(stepW * j , -stepH * i)) / square;
                texture1 = texture1 + texture(ourTexture, TexCoord + vec2(-stepW * j , -stepH * i)) / square;
                texture1 = texture1 + texture(ourTexture, TexCoord + vec2(-stepW * j , stepH * i)) / square;
            }
            else if (i > 0) {
                texture1 = texture1 + texture(ourTexture, TexCoord + vec2(stepW * j , -stepH * i)) / square;
            }
            else if (j > 0) {
                texture1 = texture1 + texture(ourTexture, TexCoord + vec2(-stepW * j , stepH * i)) / square;
            }
        }
    }

    vec4 texture2 = texture(ourTexture, TexCoord + vec2(0.002 , 0));
    //FragColor = texture(ourTexture, TexCoord);
    FragColor = vec4( texture1.x, texture1.y, texture1.z, texture1.a);
    // FragColor = vec4( texture2.x, texture1.y, texture1.z, texture2.a);
}