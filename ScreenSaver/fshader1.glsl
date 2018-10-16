#version 410

uniform vec4 translation;
uniform float outerRadius;
uniform vec2 viewDimensions;

in vec4 v2fcolor;
out vec4 myfragcolor;

void main()
{


        myfragcolor = v2fcolor;

}



