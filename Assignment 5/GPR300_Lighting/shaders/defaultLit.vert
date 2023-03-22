#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out struct Vertex
{
	vec3 WorldPosition;    
	vec2 UV;
	mat3 TBN;
}  v_out;

void main(){  
    
    //vec2 UV = v_out.UV;
    v_out.UV = vUV;
    vec3 T = vTangent;
    vec3 B;
    vec3 N = vNormal;    
    
    B = cross(N, T);

    v_out.TBN[0][0] = T.x;
    v_out.TBN[0][1] = T.y;
    v_out.TBN[0][2] = T.z;

    v_out.TBN[1][0] = B.x;
    v_out.TBN[1][1] = B.y;
    v_out.TBN[1][2] = B.z;

    v_out.TBN[2][0] = N.x;
    v_out.TBN[2][1] = N.y;
    v_out.TBN[2][2] = N.z;    

    mat3 normalMatrix = transpose(inverse(mat3(_Model)));

    v_out.TBN = normalMatrix * v_out.TBN;
    

    v_out.WorldPosition = vec3(_Model * vec4(vPos,1));
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}
