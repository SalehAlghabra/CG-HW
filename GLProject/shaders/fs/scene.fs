#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform vec4 objectColor;

// Lighting parameters
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;

void main()
{
    // Ambient component
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse component
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular component
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    vec3 specular = 0.25 * spec * lightColor;
    
    // Object Base Color
    vec4 baseColor = objectColor;
    if (useTexture)
    {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1)
            discard; // Transparency / cutouts
        baseColor = texColor * objectColor;
    }
    
    vec3 lighting = ambient + diffuse + specular;
    FragColor = vec4(lighting * vec3(baseColor), baseColor.a);
}
