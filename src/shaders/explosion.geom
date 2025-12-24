#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec4 color;
    float age;
} gs_in[];

out vec2 TexCoord;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    if (gs_in[0].age > 3.0f) return;
    
    vec4 center = gl_in[0].gl_Position;
    
    vec4 particleColor = gs_in[0].color;
    float age = gs_in[0].age;
    
    float normalizedAge = age / 3.0f;
    if (normalizedAge < 0.3f) {
        float t = normalizedAge / 0.3f;
        particleColor.rgb = mix(vec3(1.0f, 1.0f, 0.3f), vec3(1.0f, 0.5f, 0.0f), t);
    } else if (normalizedAge < 0.6f) {
        float t = (normalizedAge - 0.3f) / 0.3f;
        particleColor.rgb = mix(vec3(1.0f, 0.5f, 0.0f), vec3(1.0f, 0.1f, 0.0f), t);
    } else {
        float t = (normalizedAge - 0.6f) / 0.4f;
        particleColor.rgb = mix(vec3(1.0f, 0.1f, 0.0f), vec3(0.2f, 0.2f, 0.2f), t);
    }

    particleColor.a = max(0.0f, 1.0f - normalizedAge * 0.8f);
    
    if (particleColor.a <= 0.01f) return;

    float baseSize = 25.0f;
    float size = baseSize * (1.0f + normalizedAge * 2.0f);

    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec4 viewPos = view * center;
    
    vec4 offsets[4] = vec4[](
        viewPos + vec4(-size * cameraRight - size * cameraUp, 0.0f),
        viewPos + vec4(size * cameraRight - size * cameraUp, 0.0f),
        viewPos + vec4(-size * cameraRight + size * cameraUp, 0.0f),
        viewPos + vec4(size * cameraRight + size * cameraUp, 0.0f)
    );
    
    vec2 texCoords[4] = vec2[](
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f)
    );

    for(int i = 0; i < 4; i++)
    {
        gl_Position = projection * offsets[i];
        TexCoord = texCoords[i];
        ParticleColor = particleColor;
        EmitVertex();
    }
    
    EndPrimitive();
}
