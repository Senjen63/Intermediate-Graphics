#version 450                          
out vec4 FragColor;

in struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPosition;
}v_out;

struct Material
{
	vec3 Color;
	float AmbientK, DiffuseK, SpecularK; //(0 - 1)
	float Shininess;
};

struct Light{
    vec3 position;
    float intensity;
    vec3 color;
};



#define MAX_LIGHTS 8
//const int MAX_LIGHTS = 8;
uniform Light _Lights[MAX_LIGHTS];
uniform Material _Material;
uniform vec3 _CameraPosition;

vec3 calculateLight(Light light)
{
    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 PhongShade;

	//float ambientK = 0.1f;
	float ambientK = _Material.AmbientK;
	vec3 ambientI = _Material.Color;

	ambient = ambientK * ambientI;

	float diffuseK = _Material.DiffuseK;
	vec3 diffuseDirection = v_out.WorldPosition - light.position;
	vec3 diffuseI = light.color;

	diffuse = diffuseK * dot(normalize(diffuseDirection), normalize(v_out.WorldNormal)) * diffuseI;

	float specularK = _Material.SpecularK;
	vec3 specularDirection = v_out.WorldPosition - light.position;
	vec3 viewerDirection;
	vec3 specularI = light.color;
	
	//camera position
	//direction


	//ambient = ambientK * ambientI;

	PhongShade = ambient + diffuse + specular;

	return vec3(0);
	//return PhongShade
}

void main(){      
    vec3 normal = normalize(v_out.WorldNormal);
    FragColor = vec4(abs(normal),1.0f);
}
