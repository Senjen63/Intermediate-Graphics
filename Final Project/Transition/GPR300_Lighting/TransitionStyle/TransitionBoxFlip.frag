#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform float _Time;

//referenced by https://www.shadertoy.com/view/ltV3RG

void main(){ 

	FragColor = vec4(1);
}