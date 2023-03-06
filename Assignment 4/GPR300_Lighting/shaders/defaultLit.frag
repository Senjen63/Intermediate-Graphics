#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
/*in vec3 TBN;*/ //Assignment 5

uniform sampler2D _BrickTexture;
uniform sampler2D _Ocean;
//uniform sampler2D _NormalMap; (Assignment 5)


void main(){ 
    //vec3 normal = texture(_NormalMap,UV).rgb;
    FragColor = vec4(UV.x, UV.y, 0.0f, 1.0f);
}
