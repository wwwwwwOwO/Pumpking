#version 330 

in vec3 pos;
out vec4 color;
uniform sampler2D snowflower;

void main()
{
    vec4 texColor = texture(snowflower,gl_PointCoord);
    if(texColor.r < 0.3)discard;
    color = vec4(0.9f, 0.9f, 0.9f, texColor.a);
}
