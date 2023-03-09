#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;


out vec2 UV;
out mat3 TBN;

void main(){    
    UV = vUV;
    vec3 T = vTangent;
    vec3 B;
    vec3 N = vNormal;

    B = cross(N, T);


    TBN[0][0] = T.x;
    TBN[0][1] = T.y;
    TBN[0][2] = T.z;

    TBN[1][0] = B.x;
    TBN[1][1] = B.y;
    TBN[1][2] = B.z;

    TBN[2][0] = N.x;
    TBN[2][1] = N.y;
    TBN[2][2] = N.z;
    
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}
