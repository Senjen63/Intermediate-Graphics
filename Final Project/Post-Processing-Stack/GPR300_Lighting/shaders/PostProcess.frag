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



void Normal() {
    
    vec4 color = texture(_Texture,UV);

    FragColor = vec4(color.x, color.y, color.z, 1);
}

//White Function
void White() {
    
    vec4 color = texture(_Texture,UV);

    color = 1 -color;
    FragColor = vec4(color.x, color.y, color.z, 1);
}

//Fade to Black Function
void FadeToBlack() {
    
    float t = abs(sin(_Time));
    vec4 color = texture(_Texture,UV);

    FragColor = vec4(color.x, color.y, color.z, 1) * t;
}

//Blur Function
void Blur() {
    
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
        FragColor = vec4(color.x, color.y, color.z, 1);
}

//Sine Threshold Effect Function
void SineThresholdEffect() {
    vec2 uv;

    uv /= UV.xy;

    //float r = texture(_Texture,UV).r;
    float r = texture(_Texture,uv).r;
    //float c = step(0.0f, sin(UV.x * 10.0f + r * 40.0f));
    float c = step(0.0f, sin(uv.x * 10.0f + r * 40.0f));

    FragColor = vec4(vec3(c), 1);
}

/**************Anti-aliasing****************/
vec2 Saturate(vec2 x) {
    return x;
}

vec2 Magnify(vec2 uv) {
    return uv;
}

vec2 Quantize(vec2 uv) {
    return uv;
}

void Anti_Aliasing() {

}
/*******************************************/

void main() { 
    
    //None
    if(_Switch == 0) {
        Normal();
    }
    
    //white
    else if(_Switch == 1) {
        White();
    }

    //Fade to Black
    else if(_Switch == 2) {
        FadeToBlack();
    }

    //Blur
    else if(_Switch == 3) {
        Blur();
    }

    //Sine Threshold Effect
    else if(_Switch == 4) {
        SineThresholdEffect();
    }

    //Anti-aliasing
    else if(_Switch == 5) {
        Anti_Aliasing();
    }
}