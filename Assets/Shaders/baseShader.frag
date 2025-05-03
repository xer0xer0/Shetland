
#version 330 core

#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT 2

#define NUM_LIGHTS 2

out vec4 FragColor;
  
in vec2 UV;
in vec3 FragPos;
in mat3 TBN;

struct Light
{
    int type;
    vec3 direction;
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
uniform sampler2D albedoTex;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metalnessMap;
uniform Light lights[NUM_LIGHTS];
uniform Material material;

const float NON_METAL_COLOR = 0.04f;


vec3 CalculateDiffuse(vec3 _norm, vec3 _lightDir, vec3 _lightDiff)
{
    // the greater the angle btwn the vectors, the darker the diffuse
    float diffCoeff = max(dot(_norm, _lightDir), 0.0);
    vec3 diffuse = diffCoeff * vec3(texture(material.diffuse, UV));
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
    vec3 lightDir;

    vec3 albedo = texture(albedoTex, UV).rgb;
    vec3 normal = texture(normalMap, UV).rgb;
    float roughness = texture(roughnessMap, UV).r;
    float metalness = texture(metalnessMap, UV).r;

   // vec3 specColor = mix(NON_METAL_COLOR.rrr, albedo.rgb, metalness);
    // Check this!!
    normal =  normalize(normal * 2.0 - 1.0f);

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 ambient = lights[i].ambient * vec3(texture(material.diffuse, UV));


        if(lights[i].type == LIGHT_TYPE_POINT)
        {
            
            lightDir = normalize(lights[i].position - FragPos);
            
        }
        else if(lights[i].type == LIGHT_TYPE_DIRECTIONAL)
        {
            lightDir = normalize(-lights[i].direction);
        }

        lightDir = TBN * lightDir;

        vec3 diffuse = CalculateDiffuse(normal, lightDir, lights[i].diffuse);
        vec3 specular = CalculateSpecular(normal, lightDir, lights[i].specular);

        total += ambient + diffuse + specular;

    }

    FragColor =  texture(albedoTex, UV) * vec4(total, 1.0);
}