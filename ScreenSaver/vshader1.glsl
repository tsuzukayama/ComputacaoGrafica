# version 400

// Input vertex attributes
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColors;

// Output vertex attributes to be interpolated
// for each fragment during rasterization
out vec4 v2fcolor;

void main()
{
    // gl_Position is a mandatory built-in output variable
    gl_Position = vPosition;
    v2fcolor = vColors;
}
