#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform float _Time;

// referenced by Shadertoy(https://www.shadertoy.com/)

void main() 
{ 
    float t = abs(sin(_Time));
    vec4 color = texture(_Texture,UV);

    FragColor = vec4(color.x, color.y, color.z, 1) * t;
}