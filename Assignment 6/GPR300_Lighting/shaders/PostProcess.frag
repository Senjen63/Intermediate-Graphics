#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform int _Switch;
uniform float _Time;



void main(){ 

	vec4 color = texture(_Texture,UV);

    color = 1 -color;

    
    //white
    if(_Switch == 1)
    {
        color = 1 -color;
        FragColor = vec4(color.x, color.y, color.z, 1);
    }

    //From Yellow to Black
    else if(_Switch == 2)
    {
        float t = abs(sin(_Time));
        FragColor = vec4(color.x, color.y, 0, 1) * t;
    }

    else if(_Switch == 3)
    {
        //FragColor = vec4(color.x, color.y, color.z, 1);
    }

    else if(_Switch == 4)
    {
        //FragColor = vec4(color.x, color.y, color.z, 1);
    }

    else
    {
        //FragColor = vec4(color.x, color.y, color.z, 1);
    }
    
    
    FragColor = vec4(color.x, color.y, color.z, 1);
    
}