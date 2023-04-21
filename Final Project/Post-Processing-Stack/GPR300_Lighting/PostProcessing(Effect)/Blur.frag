#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform float _Directions;
uniform float _Quality;
uniform float _Size;

// referenced by Shadertoy(https://www.shadertoy.com/)

void main()
{ 
    vec4 color;
    float pie = 6.28318530718; //pi * 2

    for(float d = 0.0f; d < pie; d += pie/_Directions)
    {
       for(float i = 1.0/_Quality; i <= 1.0f; i += 1.0/_Quality)
       {
           color += texture(_Texture, UV + vec2(cos(d), sin(d)) * _Size * i);
       }
    }

    color /= _Quality * _Directions - 15.0f;

    FragColor = vec4(color.x, color.y, color.z, 1);
}