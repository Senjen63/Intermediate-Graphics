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

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float intensity;
	//linear and spotlight
};

struct PointLights
{
	vec3 color;
	vec3 position;
	float intensity;
	vec3 linearAttenuation;
	//linear
};

struct SpotLight
{
	vec3 color;
	vec3 position;
	vec3 direction;
	float intensity;
	vec3 linearAttenuation;
	vec3 minAngle;
	vec3 MaxAngle;
	//linear and spotlight
};



#define MAX_LIGHTS 8
//const int MAX_LIGHTS = 8;
uniform Light _Lights[MAX_LIGHTS];
uniform Material _Material;
uniform DirectionalLight _DirectionalLight;
uniform PointLights _PointLights[MAX_LIGHTS];
uniform SpotLight _SpotLight;
uniform vec3 _CameraPosition;



vec3 CalculateAmbient(vec3 ambient)
{
	float ambientK = _Material.AmbientK;
	vec3 ambientI = _Material.Color;

	ambient = ambientK * ambientI;

	return ambient;
	//return vec3(0);
}

vec3 CalculateDiffuse(Light light, vec3 diffuse)
{
	float diffuseK = _Material.DiffuseK;
	vec3 diffuseDirection = v_out.WorldPosition - light.position;
	vec3 diffuseI = light.color;

	diffuse = diffuseK * dot(normalize(diffuseDirection), normalize(v_out.WorldNormal)) * diffuseI;

	return diffuse;
}

vec3 CalculateSpecular(Light light, vec3 specular)
{
	float specularK = _Material.SpecularK;
	vec3 specularR = v_out.WorldPosition - light.position;;
	vec3 specularV = _CameraPosition - v_out.WorldPosition;
	float specularA = _Material.Shininess;
	float specularI = light.intensity;

	vec3 specularN = v_out.WorldNormal;
	vec3 specularH;
	
	vec3 specularL = v_out.WorldPosition - light.position;
	
	
	vec3 specularC = light.color;

	vec3 r = reflect(specularL, specularN);

	//vec3 specularDirection = v_out.WorldPosition - light.position;
	//vec3 specularDirection = reflect(v_out.WorldPosition - light.position);
	//R = redlect(-L,N) P(WorldPosition) = L N= WorldNormal
	//vec3 viewerDirection;
	//vec3 viewerDirection = dot(R,C);
	// Normalize(CameraP - WorldP)
	
	//V = direction towards the viewer (camera position - world position)
	//L = Direction of the light
	
	//camera position
	//direction
	//Blinn-Phong Specular
	//H = normalize(V+L)
	//|| || = magnitude

	float d = dot( normalize( specularR), normalize( specularV));

	specular = specularC * specularK * pow( d, specularA) * specularI;


	return specular;
}

vec3 CalculateBlinnPhongSpecular(Light light, vec3 specular)
{
	float specularK = _Material.SpecularK;
	vec3 specularN = v_out.WorldNormal;
	vec3 specularH;
	vec3 specularV;
	vec3 specularL;
	float specularA = _Material.Shininess;
	float specularI = light.intensity;
	vec3 specularC = light.color;

	specularV = _CameraPosition - v_out.WorldPosition;
	specularL = v_out.WorldPosition - light.position;

	specularH = normalize(specularV + specularL);

	float d = dot(normalize(specularN), normalize(specularH));

	specular = specularC * specularK * pow( d, specularA) * specularI;

	return specular;
}

vec3 calculateLight(Light light, vec3 ambient, vec3 diffuse, vec3 specular)
{
	vec3 PhongShade;

	PhongShade = CalculateAmbient(ambient) + CalculateDiffuse(light, diffuse) + CalculateBlinnPhongSpecular(light, specular);
	
	return PhongShade;
}

float FallOff()
{
 return 0;
}

void main(){      
    vec3 normal = normalize(v_out.WorldNormal);
    FragColor = vec4(abs(normal),1.0f);

	vec3 lightColor;

	for (int i = 0; i < 4; i++)
	{
		//lightColor += calculateLight(_Lights[i], _Material.AmbientK, _Material.DiffuseK, _Material.SpecularK);
	}
}
