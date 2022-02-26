#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{   
    vec4 color = textureCube(skybox, TexCoords);
    float light = -0.12f;
    color.r = max(color.r + light, 0);
    color.g = max(color.g + light, 0);
    color.b = max(color.b + light, 0);
    FragColor = color;
}