#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D ourTexture;

// Light
uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

// Material
uniform float material_gloss;

// Camera
uniform vec3 viewPos;

// Black effect after explosion
uniform bool isBlack;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);

    if (isBlack) {
        FragColor = vec4(28.0/255.0, 0.0, 0.0, texColor.a);
        return;
    }
    
    vec3 diffuseColor = texColor.rgb;
    
    // Ambient
    vec3 ambient = light_ambient * diffuseColor * 0.4;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light_diffuse * (diff * diffuseColor);
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_gloss);
    vec3 specular = light_specular * (spec * vec3(0.2));
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, texColor.a);
}
