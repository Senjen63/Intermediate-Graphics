#version 450                          
out vec4 FragColor;

in struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPosition;
}v_out;

struct Light{
    vec3 position;
    float intensity;
    vec3 color;
};
#define MAX_LIGHTS 8
//const int MAX_LIGHTS = 8;
uniform Light _Lights[MAX_LIGHTS];

vec3 calculateLight(Light light)
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    return vec3(0);
}

void main(){      
    vec3 normal = normalize(v_out.WorldNormal);
    FragColor = vec4(abs(normal),1.0f);
}
