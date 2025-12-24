#version 330 core

// TODO: Implement CubeMap shading

out vec4 FragColor;

uniform samplerCube skybox;

in vec3 TexCoord;

void main()
{
    FragColor = texture(skybox, TexCoord);
}