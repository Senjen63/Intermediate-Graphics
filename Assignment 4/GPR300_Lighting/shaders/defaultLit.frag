#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;

void main(){         
    FragColor = vec4(UV.x, UV.y, 1.0f, 1.0f);
}
