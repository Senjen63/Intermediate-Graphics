#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform bool _Invert;
uniform bool _GrayScale;
uniform bool _Wave;
uniform bool _Blur;


void main(){ 

	vec4 color = texture(_Texture,UV);

    color = 1 -color;

    

    if(_Invert)
    {
        
    }

    else if(_GrayScale)
    {
        //FragColor = vec4(color.x, color.y, color.z, 3);
    }

    else if(_Wave)
    {
        
    }

    else if(_Blur)
    {

    }

    else
    {
        //FragColor = vec4(color.x, color.y, color.z, 1);
    }
    
    FragColor = vec4(color.x, color.y, color.z, 1);
    
}