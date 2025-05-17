#version 460 core

in vec2 texCoords;
in vec3 normal;

uniform sampler2D texture_diffuse;

out vec4 fragColor;

void main()
{    
    fragColor = texture(texture_diffuse, texCoords);
}