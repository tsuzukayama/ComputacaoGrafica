#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 0) in vec4 vColors;
out vec4 v2fcolor;

uniform vec4 translation;

void main()
{
    float scaling = (translation.z - 1);
    gl_Position = (vPosition + translation) * vec4(scaling, scaling, scaling, 1);
    // gl_Position = vPosition + translation;

    vec4 color1 = vec4(1.2 - abs(translation.z), 0, abs(translation.z), 1.0);
    v2fcolor = color1;
}
