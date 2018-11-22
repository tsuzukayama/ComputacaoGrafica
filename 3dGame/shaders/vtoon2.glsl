#version 410

layout (location = 0 ) in vec3 VertexPosition;
layout (location = 1 ) in vec3 VertexNormal;

out vec3 VNormal;
out vec3 VPosition;

out vData {
    vec3 fN;
    vec3 fE;
    vec3 fL;
}vertex;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 lightPosition;

uniform mat4 model;

uniform mat3 normalMatrix;

// MVP = model view projection
void main()
{
    mat4 ModelViewMatrix = view * model;
    mat4 MVP = projection * ModelViewMatrix;

    VNormal = normalize( normalMatrix * VertexNormal);
    VPosition = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));

    vec4 eyePosition = view * model * vec4(VertexPosition, 1.0);
    vertex.fN = mat3(view) * normalMatrix * VertexNormal;
    vertex.fL = lightPosition.xyz - eyePosition.xyz;
    vertex.fE = -eyePosition.xyz;

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
