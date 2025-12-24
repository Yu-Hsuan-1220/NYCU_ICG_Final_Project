#version 330 core
in vec2 TexCoord;
in vec4 ParticleColor;

out vec4 FragColor;

void main()
{
    // Create circular particle with soft edge
    vec2 center = vec2(0.5f, 0.5f);
    float dist = distance(TexCoord, center);
    
    if(dist > 0.5f) discard; 
    
    float alpha = 1.0f - (dist / 0.5f);
    alpha = alpha * alpha * alpha;
    
    vec3 glowColor = ParticleColor.rgb * 1.5f;
    
    FragColor = vec4(glowColor, ParticleColor.a * alpha);
}
