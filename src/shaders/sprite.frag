#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D spriteTexture;

void main()
{
    // DEBUG: Draw solid red to verify quad shape
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    
    vec4 texColor = texture(spriteTexture, TexCoord);
    FragColor = texColor;
}
