
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D cobble;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float specStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specStrength * specCoeff * lightColor;

    // the greater the angle btwn the vectors, the darker the diffuse
    float diffCoeff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffCoeff * lightColor;

    vec3 combined = ambient + diffuse + specular;
    FragColor =  texture(cobble, TexCoord) * vec4(combined, 1.0);
}