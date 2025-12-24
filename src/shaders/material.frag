#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

// Light
uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

// Material (fallback if no vertex colors)
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_gloss;

// Whether to use vertex colors
uniform bool useVertexColor;

// Camera
uniform vec3 viewPos;

void main()
{
    // Use vertex color if available, otherwise use uniform material
    vec3 diffuseColor = useVertexColor ? VertexColor : material_diffuse;
    
    // For black colors, add a small ambient to make them visible
    vec3 ambientColor = diffuseColor;
    if (length(diffuseColor) < 0.1) {
        ambientColor = vec3(0.05);  // Slight gray for pure black
    }
    
    // Ambient
    vec3 ambient = light_ambient * ambientColor * 0.3;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light_diffuse * (diff * diffuseColor);
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_gloss);
    vec3 specular = light_specular * (spec * material_specular);
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
