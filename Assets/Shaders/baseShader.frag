
#version 330 core

#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT 2

#define NUM_LIGHTS 2

out vec4 FragColor;
  
in vec2 UV;
in vec3 WorldPos;
in vec3 Normal;
in vec3 Tangent;

struct Light
{
    int type;
    vec3 direction;
    vec3 position;
    vec3 color;
};

struct Material
{
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform vec3 cameraPos;
uniform sampler2D albedoTex; // fix discrepancy between Material struct and these
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metalnessMap;
uniform samplerCube irradianceMap;
uniform Light lights[NUM_LIGHTS];

// assume surface reflection const is 0.04
const float NON_METAL_COLOR = 0.04f;
const float MIN_ROUGHNESS = 0.0000001f;
const float PI = 3.14159265359f;


vec3 NormalsFromMap(vec3 _normalTex)
{
    vec3 tangentNormal = _normalTex * 2.0 - 1.0;

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Tangent);
    vec3 B  = cross(T, N);
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float SpecDistribution(vec3 _n, vec3 _h, float _roughness)
{
    float a = _roughness * _roughness;
    float a2 = a * a;
    float NDotH = max(dot(_n, _h), 0.0f);
    float NDotH2 = NDotH * NDotH;

    float denom = NDotH2 * (a2 - 1) + 1;
    denom = PI * (denom * denom);

    return a2 / denom;
}

float GeometricShadowing(vec3 _n, vec3 _v, float _roughness)
{
    float r = _roughness + 1.0;
    float k = (r * r) / 8.0;
    float NDotV = max(dot(_n, _v), 0.0);
    float denom = NDotV * (1.0 - k) + k;

    return NDotV / denom;
}

float GeometrySmith(vec3 _n, vec3 _v, vec3 _l, float _roughness)
{
    float gs1 = GeometricShadowing(_n, _v, _roughness);
    float gs2 = GeometricShadowing(_n, _l, _roughness);
    
    return gs1 * gs2;
}

vec3 FresnelSchlick(vec3 _v, vec3 _h, vec3 _specColor)
{
    float VDotH = max(dot(_h, _v), 0.0);

    return _specColor + (1.0 - _specColor) * pow(clamp(1.0 - VDotH, 0.0, 1.0), 5.0);
}

// n - normal vector, v - vector to camera, l - vector to light
vec3 CookTorrenceBRDF(vec3 _n, vec3 _v, vec3 _l, vec3 _h, float _roughness, vec3 _specColor)
{
    float D = SpecDistribution(_n, _h, _roughness);
    vec3 F = FresnelSchlick(_n, _h, _specColor);
    float G = GeometrySmith(_n, _v, _l, _roughness);

    vec3 num = D * F * G;
    float denom = 4.0 * max(dot(_n, _v), 0.0) * max(dot(_n, _l), 0.0) + 0.0001;

    return num / denom;
}

float Attenuate(Light _light)
{
    float dist = length(_light.position - WorldPos);
    float atten = 1.0 / (dist * dist);
    // TODO: add range
    return atten;
}

void main()
{
    vec3 total;
    vec3 lightDir;

    vec3 albedo = texture(albedoTex, UV).rgb;
    albedo = pow(albedo, vec3(2.2));
    vec3 normalTex = texture(normalMap, UV).rgb;
    float roughness = texture(roughnessMap, UV).r;
    float metalness = texture(metalnessMap, UV).r;

    vec3 nonMetal = vec3(NON_METAL_COLOR);
    vec3 f0 = mix(nonMetal, albedo, metalness);

    vec3 n = NormalsFromMap(normalTex);
    vec3 v = normalize(cameraPos - WorldPos);
    vec3 irradiance = texture(irradianceMap, n).rgb;

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 l;
       // vec3 radiance = lights[i].diffuse * atten;

        if(lights[i].type == LIGHT_TYPE_POINT)
        {
            
            l = normalize(lights[i].position - WorldPos);
            
        }
        else if(lights[i].type == LIGHT_TYPE_DIRECTIONAL)
        {
            l = normalize(-lights[i].direction);
        }
        
        vec3 h = normalize(v + l);
        vec3 specular = CookTorrenceBRDF(n, v, l, h, roughness, f0);
        vec3 diffuse = vec3(1.0) - FresnelSchlick(n, h, f0);
        diffuse *= 1.0 - metalness;
        float NDotL = max(dot(n, l), 0.0);

        if(lights[i].type == LIGHT_TYPE_POINT)
        {
            
            total += (diffuse * albedo / PI + specular) * NDotL * lights[i].color * Attenuate(lights[i]);
            
        }
        else if(lights[i].type == LIGHT_TYPE_DIRECTIONAL)
        {
            total += (diffuse * albedo / PI + specular) * lights[i].color * NDotL;
        }
        
    }

    vec3 ambientDiff = vec3(1.0) - FresnelSchlick(n, v, f0);
    ambientDiff *= 1.0 - metalness;
    vec3 irrDiff = irradiance * albedo;
    vec3 ambient = ambientDiff * irrDiff;

    vec3 color = ambient + total;
    color = color / (color + vec3(1.0));

    vec3 gamma = pow(color, vec3(1.0f / 2.2f));
    FragColor = vec4(gamma, 1.0f);
}