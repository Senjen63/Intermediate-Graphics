#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
/*in vec3 TBN;*/ //Assignment 5

uniform sampler2D _WoodFloor;
uniform sampler2D _Brick;
//uniform sampler2D _NormalMap; (Assignment 5)


void main(){ 
    //vec3 normal = texture(_NormalMap,UV).rgb;
    vec4 color = texture(_WoodFloor, UV);
    FragColor = vec4(color.x, color.y, color.z, 1.0f);
}
