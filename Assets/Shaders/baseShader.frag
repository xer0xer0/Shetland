
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform vec3 viewPos;
uniform sampler2D cobble;
uniform Light light;
uniform Material material;


vec3 CalculateDiffuse(vec3 _norm, vec3 _lightDir)
{
    // the greater the angle btwn the vectors, the darker the diffuse
    float diffCoeff = max(dot(_norm, _lightDir), 0.0);
    vec3 diffuse = diffCoeff * vec3(texture(material.diffuse, TexCoord));
    diffuse = diffuse * light.specular;

    return diffuse;
}

vec3 CalculateSpecular(vec3 _norm, vec3 _lightDir)
{
    float specStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-_lightDir, _norm);
    
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * specCoeff) * light.specular;

    return specular;
}

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 diffuse = CalculateDiffuse(norm, lightDir);
    vec3 specular = CalculateSpecular(norm, lightDir);

    vec3 combined = ambient + diffuse + specular;
    FragColor =  texture(cobble, TexCoord) * vec4(combined, 1.0);
}