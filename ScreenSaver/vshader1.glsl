#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 0) in vec4 vColors;
out vec4 v2fcolor;

uniform vec4 translation;

void main()
{
    float scaling = (translation.z - 1) / 2;
    gl_Position = (vPosition + translation) * vec4(scaling, scaling, scaling, 1);
    v2fcolor = vColors;
}
