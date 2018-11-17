#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

out vec3 reflectDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;

void main()
{
    // https://learnopengl.com/#!Advanced-OpenGL/Cubemaps
    vec4 worldPosition = model * vPosition;
    vec3 camPosition = vec3 (0, 0, 1);
    vec3 worldNormal = normalize(normalMatrix * vNormal);

    reflectDir = reflect(normalize(worldPosition.xyz - camPosition), worldNormal.xyz);

    gl_Position = projection * view * worldPosition;
}
