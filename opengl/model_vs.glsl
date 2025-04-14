#version 460 core
const int MAX_BONE_INFLUENCE = 4;

// 输入顶点属性
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;  // 确保这里是 ivec4
layout(location = 6) in vec4 weights;

// 骨骼变换矩阵
layout(std430, binding = 0) buffer BoneTransforms {
    mat4 finalTransforms[];
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 TexCoords;
out vec3 Normal;

void main() {
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    
    // 应用骨骼动画
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(boneIds[i] < 0) 
            continue;
            
        mat4 transform = finalTransforms[boneIds[i]];
        vec4 localPosition = transform * vec4(pos, 1.0);
        totalPosition += localPosition * weights[i];
        
        // 处理法线
        mat3 normalMatrix = transpose(inverse(mat3(transform)));
        totalNormal += normalMatrix * norm * weights[i];
    }
    
    // 如果没有骨骼影响，使用原始位置
    if(totalPosition == vec4(0.0)) {
        totalPosition = vec4(pos, 1.0);
        totalNormal = norm;
    }

    gl_Position = projection * view * model * totalPosition;
    Normal = normalize(totalNormal);
    TexCoords = tex;
}