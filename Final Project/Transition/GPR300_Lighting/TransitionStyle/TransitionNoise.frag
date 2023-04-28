#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Blank;
uniform sampler2D _Noise;
uniform float _Time;
uniform float _Speed;
uniform vec3 _Color;

//referenced by https://www.shadertoy.com/view/Mld3DN

void main(){ 
    vec4 foreward = texture(_Texture, UV);
    vec4 back = texture(_Blank, UV);
    vec4 noise = texture(_Noise, UV * 5.0);
    vec4 light = vec4(_Color, 0.5);

    float offset = sin(mod(_Time * _Speed, 3.14 * 0.5));

    float a = offset * 3.0 - UV.x;

    a = a - noise.r;

    a = clamp(a, 0.0, 1.0);

    FragColor = back * (1.0 - a) + foreward * a;
}