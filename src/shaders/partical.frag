#version 330 core
in vec2 TexCoord;
in vec4 ParticleColor;

out vec4 FragColor;

void main()
{
    // Create circular particle with soft edge
    vec2 center = vec2(0.5f, 0.5f);
    float dist = distance(TexCoord, center);
    
    if(dist > 0.5f) discard;  // Only render circle
    
    // Soft edge
    float alpha = 1.0f - (dist / 0.5f);
    alpha = alpha * alpha;  // Smooth falloff
    
    FragColor = vec4(ParticleColor.rgb, ParticleColor.a * alpha);
}
