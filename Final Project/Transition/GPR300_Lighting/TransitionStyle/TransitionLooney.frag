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

	vec2 uvC = gl_FragCoord.xy / UV.xy;

    uvC += 1.0;

    vec4 frame = texture(_Texture, vec2(uvC.x, uvC.y));

    
    
    vec2 center = UV.xy / 2.0;
    
    float d = distance(gl_FragCoord.xy, center);

    float intensity = max((d - _Radius) / (2.0 + _Blur * (1.0 + sin(_Time * _Speed))), 0.0);

    FragColor = vec4(intensity + frame.r, intensity + frame.g, intensity + frame.b, 1);
}