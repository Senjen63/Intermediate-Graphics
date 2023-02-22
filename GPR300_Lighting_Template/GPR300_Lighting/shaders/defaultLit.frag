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
	float linearAttenuation;
	float quad;
	float minAngle;
	float MaxAngle;
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



vec3 CalculateAmbient()
{
	float ambientK = _Material.AmbientK;
	vec3 ambientI = _Material.Color;

	vec3 ambient = ambientK * ambientI;

	return ambient;
	//return vec3(0);
}

vec3 CalculateDiffuse(Light light)
{
	float diffuseK = _Material.DiffuseK;
	vec3 diffuseDirection = v_out.WorldPosition - light.position;
	vec3 diffuseI = light.color;

	vec3 diffuse = diffuseK * dot(normalize(diffuseDirection), normalize(v_out.WorldNormal)) * diffuseI;

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

vec3 CalculateBlinnPhongSpecular(Light light)
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

	vec3 specular = specularC * specularK * pow( d, specularA) * specularI;

	return specular;
}

vec3 calculateLight(Light light)
{
	vec3 PhongShade;

	PhongShade = CalculateDiffuse(light) + CalculateBlinnPhongSpecular(light);
	
	return PhongShade;
}



float GLFallOff()
{
	float Distance = length(v_out.WorldPosition - _SpotLight.position);
	float I = (1 / _SpotLight.intensity + _SpotLight.linearAttenuation * Distance + _SpotLight.quad * pow(Distance, 2));

	return I;
}

float AngularAttenuation()
{
	
	//FragColor = vec4(0);

	float w = GLFallOff();

	vec3 D = (v_out.WorldPosition - _SpotLight.position) / length(v_out.WorldPosition - _SpotLight.position);

	float theta = acos(cos(dot(_SpotLight.direction, D)));

	

	float i = pow((theta - _SpotLight.MaxAngle) / (_SpotLight.minAngle - _SpotLight.MaxAngle), w);

	return i;
}

void main(){      
    vec3 normal = normalize(v_out.WorldNormal);

	vec3 lightColor = CalculateAmbient();

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		lightColor += calculateLight(_Lights[i]);
	}

    FragColor = vec4(abs(normal),1.0f);

	
}
