#version 330 core

// TODO: Implement CubeMap shading

layout (location = 0) in vec3 aPos;

// 3D direction
out vec3 TexCoord;

//out vec3 FragPos;
//out vec3 FragNormal;


//uniform mat4 model;
//uniform mat4 view;
uniform mat4 projection;

uniform mat4 view_skybox;

void main()
{
    vec4 new_pos = projection * view_skybox * vec4(aPos, 1.0);

    gl_Position = new_pos.xyww;
    TexCoord = aPos;
}  