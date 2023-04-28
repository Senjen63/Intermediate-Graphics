#version 450

out vec4 FragColor;

in vec2 UV;

uniform float _Time;
uniform float _Resolution;


//referenced by https://www.shadertoy.com/view/Xl2SRd

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(){ 

    vec2 lowResolution = vec2(floor(UV.x / _Resolution), floor(UV.y / _Resolution));
    
    if(sin(_Time) > rand(lowResolution))
    {
        FragColor = vec4(UV, 0.5 + 0.5 * sin(5.0 * UV.x), 1.0);
    }

    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
}