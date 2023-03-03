#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;

uniform sampler2D _BrickTexture;
uniform sampler2D _Ocean;

void main(){         
    FragColor = vec4(UV.x, UV.y, 0.0f, 1.0f);
}
