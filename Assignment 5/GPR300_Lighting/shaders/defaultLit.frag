#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
in mat3 TBN;
in vec3 Tangent;

uniform sampler2D _WoodFloor;
uniform sampler2D _Brick;
uniform sampler2D _NormalMap;


void main(){ 
    vec4 color = texture(_WoodFloor, UV);
    vec3 normal = texture(_NormalMap,UV).rgb;
    normal = normal * 2.0 - 1.0;

    
    FragColor = vec4(color.x, color.y, color.z, 1.0f);
}
