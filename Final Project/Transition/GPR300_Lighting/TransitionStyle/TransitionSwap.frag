#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;
uniform float _Time;
uniform float _Reflection;
uniform float _Perspective;
uniform float _depth;

//referenced by https://www.shadertoy.com/view/ltlBzn

const vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
const vec2 boundMin = vec2(0.0, 0.0);
const vec2 boundMax = vec2(1.0, 1.0);

bool inBounds (vec2 ib)
{
	return all(lessThan(boundMin, ib)) && all(lessThan(ib, boundMax));;
}

vec2 project (vec2 p)
{
	return p * vec2(1.0, -1.2) + vec2(0.0, -0.02);
}

vec4 bgColor (vec2 p, vec2 pfr, vec2 pto)
{
	vec4 c = black;

	pfr = project(pfr);

	if(inBounds(pfr))
	{
		c += mix(black, texture(_Texture, pfr), _Reflection * mix(1.0, 0.0, pfr.y));
	}

	pto = project(pto);

	if(inBounds(pto))
	{
		c += mix(black, texture(_Texture2, pto), _Reflection * mix(1.0, 0.0, pto.y));
	}

	return c;
}

void main(){ 

	
}