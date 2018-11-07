#version 410

in vec2 fTexCoord;

uniform sampler2D colorTexture;

out vec4 myfragcolor;

void main()
{
    myfragcolor = texture(colorTexture, fTexCoord);
}
