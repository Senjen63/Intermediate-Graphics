#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;
uniform float _Time;

//referenced by https://www.shadertoy.com/view/ltlBzn

void main(){ 

	float deltaTime = sin(_Time) / 2.0 + 0.5;

    float frame = ((texture(_Texture, UV).r - 0.5) + 2.0 * UV.x) / 3.0;

    float mask = smoothstep(deltaTime - 0.1, deltaTime, frame);

    float Distance = smoothstep(deltaTime - 0.3, deltaTime + 0.05, frame);

    float col = pow(smoothstep(deltaTime - 0.2, deltaTime + 0.15, frame), 3.0);

    vec3 color = texture(_Texture2, UV * (0.7 + pow(Distance, 2.0) * 0.3)).rgb;

    vec3 disColor = color * vec3(0.8, 0.4, 0.2);

    FragColor = vec4(mix(disColor, color, col) * mask, 1);
}