#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aVel;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aAge;

out VS_OUT {
    vec4 color;
    float age;
} vs_out;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    vs_out.color = aColor;
    vs_out.age = aAge;
}
