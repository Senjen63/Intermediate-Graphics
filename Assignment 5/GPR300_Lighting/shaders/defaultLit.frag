#version 450                          
out vec4 FragColor;

in struct Vertex
{
	vec3 WorldPosition;	
	vec2 UV;
	mat3 TBN;
}v_out;

struct Material
{
	vec3 color;
	float ambientK, diffuseK, specularK;
	float shininess;
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
	float maxAngle;
	float angleFallOff;
	//linear and spotlight
};

uniform sampler2D _WoodFloor;
uniform sampler2D _Brick;
uniform sampler2D _NormalMap;
uniform Material _Material;
uniform vec3 _CameraPosition;
uniform int _NumberOfLight;
uniform DirectionalLight _DirectionalLight;
uniform PointLights _PointLights;
uniform SpotLight _SpotLight;
uniform float _textureIntensity;

float GLFallOff(float linearAttenuation, float quadraticAttenuation, vec3 position)
{
	float Distance = length(v_out.WorldPosition - position);
	float I = (1 / (1 + linearAttenuation * Distance + quadraticAttenuation * pow(Distance, 2)));

	return I;
}

float AngularAttenuation(SpotLight spotLight) // point light and spot light
{	
	float w = spotLight.angleFallOff;

	vec3 D = normalize(v_out.WorldPosition - spotLight.position);

	float theta = dot(normalize(spotLight.direction), D);

	spotLight.minAngle = cos(radians(spotLight.minAngle));
	spotLight.maxAngle = cos(radians(spotLight.maxAngle));
	
	float i = pow(clamp((theta - spotLight.maxAngle) / (spotLight.minAngle - spotLight.maxAngle),0 ,1), w);

	return i;
}

vec3 CalculateAmbient()
{
	float ambientK = _Material.ambientK;
	vec3 ambientI = _Material.color;

	vec3 ambient = ambientK * ambientI;

	return ambient;
}

vec3 CalculateDiffuse(vec3 lightDirection, vec3 lightColor, vec3 normal)
{
	float diffuseK = _Material.diffuseK;
	
	vec3 diffuseI = lightColor;

	vec3 diffuse = diffuseK * max(dot(normalize(lightDirection), normalize(normal)), 0) * diffuseI;

	return diffuse;
}

vec3 CalculateBlinnPhongSpecular(vec3 directionTowardLight, vec3 color, float intensity, vec3 normal)
{
	float specularK = _Material.specularK;
	vec3 specularN = normal;
	vec3 specularH;
	vec3 specularV;
	vec3 specularL;
	float specularA = _Material.shininess;
	float specularI = intensity;
	vec3 specularC = color;

	specularV = _CameraPosition - v_out.WorldPosition; //Direction

	specularL = directionTowardLight;// direction to the light

	specularH = normalize(specularV + specularL);

	float d = max(dot(normalize(specularN), normalize(specularH)), 0);

	vec3 specular = specularC * specularK * pow( d, specularA) * specularI;

	return specular;
}

vec3 CalculatePointLight(PointLights pointLight, vec3 normal)
{
	vec3 phongShade;

	vec3 diffuseDirection = pointLight.position - v_out.WorldPosition;

	vec3 ambientColor = pointLight.color * _Material.ambientK;

	phongShade = ambientColor + CalculateDiffuse(diffuseDirection, pointLight.color, normal) 
	+ CalculateBlinnPhongSpecular(diffuseDirection, pointLight.color, pointLight.intensity, normal);

	phongShade *= GLFallOff(pointLight.linearAttenuation, pointLight.quadractic, pointLight.position);
	
	return phongShade ;
}

vec3 CalculateDirectionalLights(DirectionalLight directionalLight, vec3 normal)
{
	vec3 phongShade;

	vec3 DirectionTowardsLight = -directionalLight.direction;
	vec3 ambientColor = directionalLight.color * _Material.ambientK;

	phongShade = ambientColor + CalculateDiffuse(DirectionTowardsLight, directionalLight.color, normal) 
	+ CalculateBlinnPhongSpecular(DirectionTowardsLight, directionalLight.color, directionalLight.intensity, normal);
	
	return phongShade;
}

vec3 CalculateSpotLight(SpotLight spotLight, vec3 normal)
{
	vec3 phongShade;

	vec3 diffuseDirection = spotLight.position - v_out.WorldPosition;
	vec3 ambientColor = spotLight.color * _Material.ambientK;

	phongShade = ambientColor + CalculateDiffuse(diffuseDirection, spotLight.color, normal) 
	+ CalculateBlinnPhongSpecular(diffuseDirection, spotLight.color, spotLight.intensity, normal);

	phongShade *= AngularAttenuation(spotLight) * GLFallOff(spotLight.linearAttenuation, spotLight.quadractic, spotLight.position);
	
	return phongShade;
}


void main(){ 
	vec3 lightColor  = vec3(0);
	vec3 normal = texture(_NormalMap,v_out.UV).rgb;
	normal = normal * 2.0 - 1.0;
    normal = normalize(normal);

	lightColor += CalculatePointLight(_PointLights, normal);
	lightColor += CalculateDirectionalLights(_DirectionalLight, normal) + CalculateSpotLight(_SpotLight, normal) * AngularAttenuation(_SpotLight);
    vec4 color = texture(_WoodFloor, v_out.UV);
	color.r = color.r * _textureIntensity;
    
	//FragColor = vec4(color.x, color.y, color.z, 1.0f);
	//FragColor = vec4(color.x, color.y, color.z, 1.0f) * vec4(_Material.color, 1);
    FragColor = vec4(color.x, color.y, color.z, 1.0f) * vec4(_Material.color * lightColor, 1);
}
