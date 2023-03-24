#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;

void main(){ 

	vec4 color = texture(_Texture,UV);

    color = 1 -color;
    
    FragColor = vec4(color.x, color.y, color.z, 1);
    //FragColor = vec4(UV.x, UV.y, 1, 1);
}