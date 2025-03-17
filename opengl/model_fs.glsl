#version 460 core
in vec2 TexCoords;
uniform sampler2D texture_diffuse;
out vec4 FragColor;
void main()
{    
    FragColor = texture(texture_diffuse, TexCoords);
}