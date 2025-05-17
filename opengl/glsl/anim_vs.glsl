#version 460 core
const int MAX_BONE_INFLUENCE = 4;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

layout(std430) buffer BoneTrans { mat4 finalTransforms[]; };

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;
out vec3 normal;

void main() 
{
    vec4 finalPos = vec4(0.0);
    vec3 finalNorm = vec3(0.0);
    
    for(int i = 0; i < MAX_BONE_INFLUENCE; ++i) 
    {
        if(-1 == boneIds[i])continue;
        mat4 finalTrans = finalTransforms[boneIds[i]];
        finalPos += finalTrans * vec4(pos, 1.0) * weights[i];
        finalNorm += mat3(finalTrans) * norm * weights[i];
    }

    if(length(finalNorm) > 1e-6)
        finalNorm = normalize(finalNorm);
    else
        finalNorm = vec3(0.0, 0.0, 1.0);

    gl_Position = projection * view * model * finalPos;
    normal = finalNorm;
    texCoords = tex;
}