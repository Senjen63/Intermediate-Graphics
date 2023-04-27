#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform float _Speed;
uniform float _Blur;
uniform float _Radius;
uniform float _Time;

//referenced by https://www.shadertoy.com/view/lddXWS

void main(){ 

	vec4 frame = texture(_Texture, UV);
    
    vec2 center = vec2(0.5);
    
    float d = distance(UV, center);

    float intensity = max((d - _Radius) / (2.0 + _Blur * (1.0 + sin(_Time * _Speed))), 0.0);

    FragColor = vec4(intensity + frame.r, intensity + frame.g, intensity + frame.b, 1);
}