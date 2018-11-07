#version 410

// input variables processed in parallel
layout (location = 0) in vec4 vPosition;
layout (location = 2) in vec2 vTexCoord;

// output variable interpolated at fragment shader raster
out vec2 fTexCoord;

uniform float scaling;
uniform vec4 translation;

void main()
{
   //gl_Position->out builtin variable
   gl_Position = (vPosition * vec4(scaling, scaling, scaling, 1)) + translation;
   fTexCoord = vTexCoord;
}
