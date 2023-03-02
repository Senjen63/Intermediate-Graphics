#version 450                          
out vec4 FragColor;

uniform vec3 _Color;
uniform sampler2D _BrickTexture;
uniform sampler2D _Ocean;

void main(){         
    FragColor = vec4(_Color,1.0f);
}
