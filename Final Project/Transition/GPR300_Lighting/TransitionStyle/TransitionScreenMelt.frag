#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;
uniform float _Time;
uniform float _StartSpeed; // initial speed of "paint melt"
uniform float _Speed; // speed of texture melting off screen 
uniform float _Restart; // melt effect restart interval (seconds)

//referenced by https://www.shadertoy.com/view/XtlyDn

void main(){ 
    
    vec2 paint = UV;

    float deltaTime = mod(_Time, _Restart);

    float restartTime = mod(_Time, 2.0 * _Restart); // flip textures every second it melt

    bool textureFlip = restartTime > 0.0 && restartTime < _Restart;

    float drip = _StartSpeed * deltaTime; // let some "paint" drip before moving entire texture contents

    if(drip > 1.0)
    {
        drip = 1.0;
    }

     // initial shift of paint melt 
    paint.y += drip * 0.35 * fract(sin(dot(vec2(paint.x, .0), vec2(12.9898, 78.233)))* 43758.5453);

    // moves entire melted texture offscreen
    if(drip == 1.0)
    {
        paint.y += _Speed * (deltaTime - drip / _StartSpeed);
    }

    if(textureFlip)
    {
        FragColor = texture(_Texture, paint);
    }

    else
    {
        FragColor = texture(_Texture2, paint);
    }

     // draws second image behind the melting texture

     if (paint.y > 1.0)
     {
        if(textureFlip)
        {
            FragColor = texture(_Texture2, UV);
        }

        else
        {
            FragColor = texture(_Texture, UV);
        }
     }

     

    
}