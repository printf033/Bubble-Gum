#version 460 core
const int MAX_BONE_INFLUENCE = 4;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;
out vec3 normal;

void main() 
{
    gl_Position = projection * view * model * vec4(pos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    normal = normalize(normalMatrix * norm);
    texCoords = tex;
}