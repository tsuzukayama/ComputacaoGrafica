#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;
uniform vec4 lightPosition;

out vData {
    vec3 fN;
    vec3 fE;
    vec3 fL;
    vec2 ftexCoord;
}vertex;

void main()
{
    vec4 eyePosition = view * model * vPosition;
    vertex.fN = mat3(view) * normalMatrix * vNormal;
    vertex.fL = lightPosition.xyz - eyePosition.xyz;
    vertex.fE = -eyePosition.xyz;
    gl_Position = projection * eyePosition;
    vertex.ftexCoord = vTexCoords;
}
