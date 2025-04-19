
#version 330 core

#define NUM_LIGHTS 2

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Light
{
    int type;
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
uniform Light lights[NUM_LIGHTS];
uniform Material material;


vec3 CalculateDiffuse(vec3 _norm, vec3 _lightDir, vec3 _lightDiff)
{
    // the greater the angle btwn the vectors, the darker the diffuse
    float diffCoeff = max(dot(_norm, _lightDir), 0.0);
    vec3 diffuse = diffCoeff * vec3(texture(material.diffuse, TexCoord));
    diffuse = diffuse * _lightDiff;

    return diffuse;
}

vec3 CalculateSpecular(vec3 _norm, vec3 _lightDir, vec3 _lightSpec)
{
    float specStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-_lightDir, _norm);
    
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * specCoeff) * _lightSpec;

    return specular;
}

void main()
{
    vec3 total;

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        if(lights[i].type == 0)
        {
            vec3 ambient = lights[i].ambient * vec3(texture(material.diffuse, TexCoord));

            vec3 norm = normalize(Normal);

            vec3 lightDir = normalize(lights[i].position - FragPos);

            vec3 diffuse = CalculateDiffuse(norm, lightDir, lights[i].diffuse);
            vec3 specular = CalculateSpecular(norm, lightDir, lights[i].specular);

            total += ambient + diffuse + specular;
        }

    }

    FragColor =  texture(cobble, TexCoord) * vec4(total, 1.0);
}