#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform int _Switch;

//true or false controller
uniform bool _IsNone;
uniform bool _IsWhite;
uniform bool _IsFadeToBlack;
uniform bool _IsBlur;
uniform bool _IsSineThresholdEffect;

//Fade to Black
uniform float _Time;

//Blur Variables
uniform float _Directions;
uniform float _Quality;
uniform float _Size;

//Screen Space Reflection Variables



vec4 Normal() {
    
    vec4 color = texture(_Texture,UV);

    return vec4(color.x, color.y, color.z, 1);
}

// referenced by Shadertoy(https://www.shadertoy.com/)
//White Function
vec4 White() {
    
    vec4 color = texture(_Texture,UV);

    color = 1 -color;

    return vec4(color.x, color.y, color.z, 1);
}

//Blur Function
vec4 Blur() {
    
    vec4 color;

    float pie = 6.28318530718; //pi * 2

    for(float d = 0.0f; d < pie; d += pie/_Directions)
    {
       for(float i = 1.0/_Quality; i <= 1.0f; i += 1.0/_Quality)
       {
           color += texture(_Texture, UV + vec2(cos(d), sin(d)) * _Size * i);
       }
    }
        color /= _Quality * _Directions - 15.0f;

        return vec4(color.x, color.y, color.z, 1);
}

//Sine Threshold Effect Function
vec4 SineThresholdEffect() {
    
    float r = texture(_Texture,UV).r;
    float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));

    return vec4(vec3(c), 1);
}

void main() { 
    //None
    if(_Switch == 0) {
       FragColor = Normal();
    }
    
    //white
    else if(_Switch == 1) {
        FragColor = White();
    }

    //Fade to Black
    else if(_Switch == 2) {
        float t = abs(sin(_Time));
        FragColor = Normal() * t;
    }

    //Blur
    else if(_Switch == 3) {
        FragColor = Blur();
    }

    //Sine Threshold Effect
    else if(_Switch == 4) {
        FragColor = SineThresholdEffect();
    }

    //White + Fade to Black
    else if(_Switch == 5) {
        float t = abs(sin(_Time));
        vec4 color = texture(_Texture,UV);
        color = 1 -color;
        FragColor = vec4(color.x, color.y, color.z, 1) * t;
    }

    else if(_Switch == 6) {
        float t = abs(sin(_Time));
        float r = texture(_Texture,UV).r;
        float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
        FragColor = vec4(vec3(c), 1) * t;
    }

    else if(_Switch == 7) {
        float t = abs(sin(_Time));
        float r = texture(_Texture,UV).r;
        float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
        vec4 color = texture(_Texture,UV);
        color = 1 -color;

        FragColor = vec4(color.xyz * vec3(c), 1) * t;
    }

    else if(_Switch == 8) {
        float r = texture(_Texture,UV).r;
        float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
        vec4 color = texture(_Texture,UV);
        color = 1 -color;

        FragColor = vec4(color.xyz * vec3(c), 1);
    }

    else if(_Switch == 9) {
        vec4 color;

        float pie = 6.28318530718; //pi * 2

        for(float d = 0.0f; d < pie; d += pie/_Directions)
        {
            for(float i = 1.0/_Quality; i <= 1.0f; i += 1.0/_Quality)
            {
                color += texture(_Texture, UV + vec2(cos(d), sin(d)) * _Size * i);
            }
        }
        color /= _Quality * _Directions - 15.0f;
        color = 1 -color;

        FragColor = vec4(color.x, color.y, color.z, 1);
    }

    else if(_Switch == 10) {
        float r = texture(_Texture,UV).r;
        //float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
        float pie = 6.28318530718; //pi * 2

        for(float d = 0.0f; d < pie; d += pie/_Directions)
        {
            for(float i = 1.0/_Quality; i <= 1.0f; i += 1.0/_Quality)
            {
                r += texture(_Texture, UV + vec2(cos(d), sin(d)) * _Size * i).r;
            }
        }

        
        r /= _Quality * _Directions - 15.0f;

        float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));

        FragColor = vec4(vec3(c), 1);
    }
}