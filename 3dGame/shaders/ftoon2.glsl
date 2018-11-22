#version 410
//*** Light and material uniforms go here ****

in vec3 gL; // Position in camera coords
in vec3 gN; // Normal in camera coords.

uniform vec4 ambientProduct;
uniform vec4 diffuseProduct;
uniform vec4 specularProduct;
uniform float shininess;
uniform vec4 lightPosition;

flat in float GIsEdge; // Whether or not we're drawing an edge

const int levels = 3;
const float scaleFactor = 1.0 / levels;

layout( location = 0 ) out vec4 frag_color;

float Toon(vec3 l, vec3 n)
{
    const int levels = 3;

    float cosine = max(0.0, dot(l, n));
    return floor(cosine * levels) / levels;
}

vec4 toonShade()
{
    vec3 N = normalize(gN);
    vec3 L = normalize(gL);

    float NdotL = dot(N, L);
    vec3 R = normalize(2.0 * NdotL * N - L);
    float Kd = max(NdotL, 0.0);

    vec3 s = normalize( lightPosition.xyz - gL.xyz );
    float cosine = max( 0.0, dot(s, gN));
    vec4 diffuse = diffuseProduct * Kd * floor( cosine * levels ) *
     scaleFactor;
    return specularProduct * (ambientProduct + diffuse);
}

void main()
    {
    // If we're drawing an edge, use constant color,
    // otherwise, shade the poly.
    if(GIsEdge > 0.5) {
        frag_color = vec4(1, 1, 1, 1);
    } else {
        frag_color = toonShade();
    }
}
