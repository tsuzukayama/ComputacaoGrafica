#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vData {
    vec3 fN;
    vec3 fE;
    vec3 fL;
    vec2 ftexCoord;
}vertices[];

out fData {
    vec3 fN;
    vec3 fE;
    vec3 fL;
    vec2 ftexCoord;
}frag;

void main() {
  for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
    frag.fN = vertices[i].fN;
    frag.fE = vertices[i].fE;
    frag.fL = vertices[i].fL;
    frag.ftexCoord = vertices[i].ftexCoord;
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
