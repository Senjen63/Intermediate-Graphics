#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;

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


void main(){ 

	vec4 color;

    //None
    color = texture(_Texture,UV);
    FragColor = vec4(color.x, color.y, color.z, 1);
    
    //white
    //color = texture(_Texture,UV);
    //color = 1 -color;
    //FragColor = vec4(color.x, color.y, color.z, 1);
    

    //Sine Threshold Effect
    //float r = texture(_Texture,UV).r;
    //float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
    //FragColor = vec4(vec3(c), 1);
}