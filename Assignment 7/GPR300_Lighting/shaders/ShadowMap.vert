#version 450
layout (location = 0) in vec3 vPos;
layout (location = 2) in vec2 vUV;

uniform mat4 _ModelViewProjection;

void main(){    
    
    gl_Position = _ModelViewProjection * vec4(vPos,1);
}