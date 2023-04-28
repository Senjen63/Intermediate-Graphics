#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform float _Time;

//referenced by https://www.shadertoy.com/view/ltV3RG

vec3 TextureVisual1(vec2 uv)
{
    return texture(_Texture, uv).rgb;
}

vec3 TextureVisual2(vec2 uv)
{
    return texture(_Texture, uv).rrr;
}

float Hash(vec2 hash)
{
    vec3 hash2 = vec3(hash.xy, 1.0);

    return fract(sin(dot(hash2, vec3(37.1, 61.7, 12.4))) * 3758.5453123);
}

float Noise (vec2 noise)
{
    vec2 i = floor(noise);
    vec2 f = fract(noise);

    f *= f * (3.0 - 2.0 * f);

    return mix(mix(Hash(i + vec2(0.0, 0.0)), Hash(i + vec2(1.0, 0.0)), f.x), mix(Hash(i + vec2(0.0, 1.0)), Hash(i + vec2(1.0, 1.0)),f.x),f.y);
}

float fbm(vec2 p) 
{
	float v = 0.0;
	v += Noise(p * 1.0) * 0.5;
	v += Noise(p * 2.0) * 0.25;
	v += Noise(p * 4.0) * 0.125;
	return v;
}


void main(){ 

	vec3 color;
    vec2 uvT = UV;

    vec3 texture1 = TextureVisual1(uvT);
    vec3 texture2 = TextureVisual2(uvT);

    color = texture1;

    uvT.x -= 1.5;

    float deltaTime = mod(_Time * 0.5, 2.5);

    float d = uvT.x + uvT.y * 0.5 + 0.5 * fbm(uvT * 15.1) + deltaTime * 1.3;

    if(d > 0.35)
    {
        color = clamp(color - (d - 0.35) * 10.0, 0.0, 1.0);
    }

    if (d > 0.47)
    {
        if (d < 0.5)
        {
            color += (d - 0.4) * 33.0 * 0.5 * (0.0 + Noise(100.0 * uvT + vec2(-deltaTime * 2.0, 0.0))) * vec3(1.5, 0.5, 0.0);
        }

        else
        {
            color += texture2;
        }
    }

    FragColor = vec4(color, 1);
}