#version 400
//*** Light and material uniforms go here ****

uniform vec4 LineColor; // The sil. edge color

in vec3 gL; // Position in camera coords
in vec3 gN; // Normal in camera coords.

flat in bool GIsEdge; // Whether or not we're drawing an edge
layout( location = 0 ) out vec4 FragColor;

vec3 toonShade( )
{
 // *** toon shading algorithm from Chapter 3 ***
}
void main()
    {
    // If we're drawing an edge, use constant color,
    // otherwise, shade the poly.
    if( GIsEdge ) {
        FragColor = LineColor;
    } else {
        FragColor = vec4( toonShade(), 1.0 );
    }
}
