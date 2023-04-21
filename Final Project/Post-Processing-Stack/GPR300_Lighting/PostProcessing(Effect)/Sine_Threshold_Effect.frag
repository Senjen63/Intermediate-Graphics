#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;

// referenced by Shadertoy(https://www.shadertoy.com/)
void main() 
{ 
    float r = texture(_Texture,UV).r;
    float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));

    FragColor = vec4(vec3(c), 1);
}