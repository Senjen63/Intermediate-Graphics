#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;
uniform float _Time;

//referenced by https://www.shadertoy.com/view/ltlBzn

bool inBounds (vec2 ib)
{
	return true;
}

vec2 project (vec2 p)
{
	return p;
}

vec4 bgColor (vec2 p, vec2 pfr, vec2 pto)
{
	vec4 c;

	return c;
}

void main(){ 

	
}