#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform int _Switch;

//Fade to Black
uniform float _Time;

//Blur Variables
uniform float _Directions;
uniform float _Quality;
uniform float _Size;



void main(){ 

	vec4 color;

    //None
    if(_Switch == 0)
    {
        color = texture(_Texture,UV);
        FragColor = vec4(color.x, color.y, color.z, 1);
        //FragColor = vec4(UV.x, UV.y, 0, 1);

    }
    
    //white
    else if(_Switch == 1)
    {
        color = texture(_Texture,UV);
        color = 1 -color;
        FragColor = vec4(color.x, color.y, color.z, 1);
    }

    //Fade to Black
    else if(_Switch == 2)
    {
        float t = abs(sin(_Time));
        color = texture(_Texture,UV);
        FragColor = vec4(color.x, color.y, color.z, 1) * t;
    }

    //Blur
    else if(_Switch == 3)
    {
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

    //Sine Threshold Effect
    else if(_Switch == 4)
    {
        float r = texture(_Texture,UV).r;
        float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
        FragColor = vec4(vec3(c), 1);
    }

    
    
}