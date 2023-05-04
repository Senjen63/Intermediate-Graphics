#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;
uniform float _Time;
uniform float _Reflection;
uniform float _Perspective;
uniform float _Depth;
uniform vec3 _Background;

//referenced by https://www.shadertoy.com/view/MlXGzf

const vec2 boundMin = vec2(0.0, 0.0);
const vec2 boundMax = vec2(1.0, 1.0);

bool inBounds (vec2 ib)
{
	return all(lessThan(boundMin, ib)) && all(lessThan(ib, boundMax));
}

vec2 project (vec2 p)
{
	return p * vec2(1.0, -1.2) + vec2(0.0, -0.02);
}

vec4 bgColor (vec2 pfr, vec2 pto)
{
	vec4 c = vec4(_Background, 1.0);

	pfr = project(pfr);

	if(inBounds(pfr))
	{
		c += mix(vec4(_Background, 1.0), texture(_Texture, pfr), _Reflection * mix(1.0, 0.0, pfr.y));
	}

	pto = project(pto);

	if(inBounds(pto))
	{
		c += mix(vec4(_Background, 1.0), texture(_Texture2, pto), _Reflection * mix(1.0, 0.0, pto.y));
	}

	return c;
}

void main(){ 
	float deltaTime = sin(_Time * .5) * .5 + .5;

	vec2 pfr;
	vec2 pto = vec2(-1.0);

	float size = mix(1.0, _Depth, deltaTime);
	float perspect = _Perspective * deltaTime;

	pfr = (UV + vec2(-0.0, -0.5)) * vec2(size / (1.0 - _Perspective * deltaTime),
	size / (1.0 - size * perspect * UV.x)) + vec2(0.0, 0.5);

	size = mix(1.0, _Depth, 1.0 - deltaTime);

	perspect = _Perspective * (1.0 - deltaTime);

	pto = (UV + vec2(-1.0, -0.5)) * vec2(size / (1.0 - _Perspective * (1.0 - deltaTime)),
	size / (1.0 - size * perspect * (0.5 - UV.x))) + vec2(1.0, 0.5);

	bool Switch = deltaTime < 0.5;

	if(Switch)
	{
		if(inBounds(pfr))
		{
			FragColor = texture(_Texture, pfr);
		}

		else if(inBounds(pto))
		{
			FragColor = texture(_Texture2, pto);
		}

		else
		{
			FragColor = bgColor(pfr, pto);
		}
	}

	else
	{
		if(inBounds(pto))
		{
			FragColor = texture(_Texture2, pto);
		}

		else if(inBounds(pfr))
		{
			FragColor = texture(_Texture, pfr);
		}

		else
		{
			FragColor = bgColor(pfr, pto);
		}
	}
}