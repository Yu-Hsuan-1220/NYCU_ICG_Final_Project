#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 burnCenter;      
uniform float burnRadius;     
uniform bool burnEnabled; 

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    
    if (burnEnabled) {
       
        float dist = distance(vec2(FragPos.x, FragPos.z), vec2(burnCenter.x, burnCenter.z));
        
        if (dist < burnRadius) {
            
            float burnFactor = 1.0 - (dist / burnRadius); 
            burnFactor = pow(burnFactor, 0.5); 
            burnFactor = clamp(burnFactor * 1.5, 0.0, 1.0); 
            vec3 burnColor = mix(texColor.rgb, vec3(0.0, 0.0, 0.0), burnFactor);
            FragColor = vec4(burnColor, texColor.a);
        } else {
            FragColor = texColor;
        }
    } else {
        FragColor = texColor;
    }
} 