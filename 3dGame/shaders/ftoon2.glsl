#version 410
//*** Light and material uniforms go here ****

uniform vec4 lightPosition;
uniform vec4 ambientProduct;
uniform vec4 diffuseProduct;
uniform vec4 specularProduct;

uniform float shininess;

uniform vec3 materialKd;
uniform vec3 materialKa;
uniform vec3 lightIntensity;

in fData {
    vec3 fN;
    vec3 fE;
    vec3 fL;
}frag;

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge; // Whether or not we're drawing an edge

layout( location = 0 ) out vec4 frag_color;

const int levels = 3;
const float scaleFactor = 1.0 / levels;

vec3 toonShade()
{    
    vec3 N = normalize(frag.fN);
    vec3 E = normalize(frag.fE);
    vec3 L = normalize(frag.fL);

    float NdotL = dot(N, L);
    vec3 R = normalize(2.0 * NdotL * N - L);
    float Kd = max(NdotL, 0.0);
    float Ks = (NdotL < 0.0) ? 0.0 : pow(max(dot(R, E), 0.0), shininess);

    vec3 s = normalize( lightPosition.xyz - GPosition.xyz );
    vec3 ambient = ambientProduct.xyz;
    float cosine = max( 0.0, dot(s, GNormal));
    vec3 diffuse = Kd * diffuseProduct.xyz * ceil( cosine * levels ) * scaleFactor;
    vec3 specular = Ks * specularProduct.xyz;

    return lightIntensity * (ambient + diffuse + specular);
}

void main()
    {
    // If we're drawing an edge, use constant color,
    // otherwise, shade the poly.
    if(GIsEdge == 1) {
        frag_color = vec4(0.05f, 0.0f, 0.05f, 1.0f);
    } else {
        frag_color = vec4(toonShade(), 1.0);
    }
}
