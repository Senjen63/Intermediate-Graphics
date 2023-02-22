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
	float linearAttenuation;
	float quadractic;
	//linear
};

struct SpotLight
{
	vec3 color;
	vec3 position;
	vec3 direction;
	float intensity;
	float linearAttenuation;
	float quadractic;
	float minAngle;
	float MaxAngle;
	//linear and spotlight
};



#define MAX_LIGHTS 8
//const int MAX_LIGHTS = 8;
uniform Material _Material;

uniform vec3 _CameraPosition;
uniform int _NumberOfLight;
uniform DirectionalLight _DirectionalLight[MAX_LIGHTS];
uniform PointLights _PointLights[MAX_LIGHTS];
uniform SpotLight _SpotLight[MAX_LIGHTS];



vec3 CalculateAmbient()
{
	float ambientK = _Material.AmbientK;
	vec3 ambientI = _Material.Color;

	vec3 ambient = ambientK * ambientI;

	return ambient;
	//return vec3(0);
}

vec3 CalculateDiffuse(vec3 lightDirection, vec3 lightColor)
{
	float diffuseK = _Material.DiffuseK;
	
	vec3 diffuseI = lightColor;

	vec3 diffuse = diffuseK * dot(normalize(lightDirection), normalize(v_out.WorldNormal)) * diffuseI;

	return diffuse;
}

vec3 CalculateSpecular(PointLights pointLight, vec3 specular)
{
	float specularK = _Material.SpecularK;
	vec3 specularR = v_out.WorldPosition - pointLight.position;;
	vec3 specularV = _CameraPosition - v_out.WorldPosition;
	float specularA = _Material.Shininess;
	float specularI = pointLight.intensity;

	vec3 specularN = v_out.WorldNormal;
	vec3 specularH;
	
	vec3 specularL = v_out.WorldPosition - pointLight.position;
	
	
	vec3 specularC = pointLight.color;

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

vec3 CalculateBlinnPhongSpecular(vec3 directionTowardLight, vec3 color, float intensity) //
{
	float specularK = _Material.SpecularK;
	vec3 specularN = v_out.WorldNormal;
	vec3 specularH;
	vec3 specularV;
	vec3 specularL;
	float specularA = _Material.Shininess;
	float specularI = intensity;
	vec3 specularC = color;

	specularV = _CameraPosition - v_out.WorldPosition; //Direction

	specularL = directionTowardLight;// direction to the light

	specularH = normalize(specularV + specularL);

	float d = dot(normalize(specularN), normalize(specularH));

	vec3 specular = specularC * specularK * pow( d, specularA) * specularI;

	return specular;
}

vec3 calculatePointLight(PointLights pointLight)
{
	vec3 PhongShade;

	vec3 diffuseDirection = pointLight.position - v_out.WorldPosition;

	PhongShade = CalculateDiffuse(diffuseDirection, pointLight.color) + CalculateBlinnPhongSpecular(pointLight);
	
	return PhongShade;
}

vec3 calculateDirectionalLights(DirectionalLight directionalLight)
{
	vec3 PhongShade;

	vec3 DirectionTowardsLight = -directionalLight.direction

	PhongShade = CalculateDiffuse(DirectionTowardsLight, directionalLight.color) + CalculateBlinnPhongSpecular(directionalLight);
	
	return PhongShade;
}

vec3 calculateSpotLight(SpotLight spotLight)
{
	vec3 PhongShade;

	PhongShade = CalculateDiffuse(spotLight) + CalculateBlinnPhongSpecular(spotLight);
	
	return PhongShade;
}




float GLFallOff(float linearAttenuation, float quadraticAttenuation, vec3 position)
{
	float Distance = length(v_out.WorldPosition - position);
	float I = (1 / 1 + linearAttenuation * Distance + quadraticAttenuation * pow(Distance, 2));

	return I;
}

float AngularAttenuation(SpotLight spotLight)// point light and spot light
{
	
	//FragColor = vec4(0);

	float w = GLFallOff();

	vec3 D = (v_out.WorldPosition - spotLight.position) / length(v_out.WorldPosition - spotLight.position);

	float theta = dot(normalize(spotLight.direction), D);

	

	float i = pow((theta - spotLight.MaxAngle) / (spotLight.minAngle - spotLight.MaxAngle), w);

	return i;
}

void main(){      
    vec3 normal = normalize(v_out.WorldNormal);

	vec3 lightColor = CalculateAmbient();

	for (int i = 0; i < _NumberOfLight; i++)
	{
		lightColor += calculateLight(_PointLights[i]);
	}

	for (int i = 0; i < _NumberOfLight; i++)
	{
		lightColor += calculateLight(_DirectionalLight[i]);
	}

	for (int i = 0; i < _NumberOfLight; i++)
	{
		lightColor += calculateLight(_SpotLight[i]);
	}

    FragColor = vec4(_Material.Color * lightColor, 1);

	
}
