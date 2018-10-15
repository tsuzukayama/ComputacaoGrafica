#version 410
in vec4 v2fcolor;
out vec4 myfragcolor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec3 color1 = vec3(1.9,0.55,0);
    vec3 color2 = vec3(0.226,0.000,0.615);

    float mixValue = distance(vec2(rand(vec2(0.5,0.2)), rand(vec2(0.5,0.9))),vec2(0,0.1));
    vec3 color = mix(color1,color2,mixValue);
    myfragcolor = vec4(color, mixValue);
    // myfragcolor = v2fcolor;
}



