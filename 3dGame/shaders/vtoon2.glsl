#version 410

layout (location = 0 ) in vec4 vPosition;
layout (location = 1 ) in vec3 vNormal;

out vec3 vN;
out vec3 vL;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform mat3 normalMatrix;
uniform vec4 lightPosition;

// MVP = model view projection
void main()
{
    mat4 ModelViewMatrix = model * view;
    mat4 MVP = model * view * projection;

    vN = normalize( normalMatrix * vNormal);
    vL = vec3(ModelViewMatrix * vPosition);
    gl_Position = MVP * vPosition;
}
