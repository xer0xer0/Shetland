
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

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfMap;

uniform sampler2D albedoTex; // fix discrepancy between Material struct and these
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metalnessMap;

uniform Light lights[NUM_LIGHTS];

// assume surface reflection const is 0.04
const float NON_METAL_COLOR = 0.04f;
const float MIN_ROUGHNESS = 0.0000001f;
const float PI = 3.14159265359f;
const float MAX_REFLECTION_LOD = 4.0f;

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

vec3 FresnelSchlick(float _cosT, vec3 _specColor)
{
    return _specColor + (1.0 - _specColor) * pow(clamp(1.0 - _cosT, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float _cosT, vec3 _specColor, float _roughness)
{
    vec3 r = max(vec3(1.0f - _roughness), _specColor);
    return _specColor + (r - _specColor) * pow(clamp(1.0 - _cosT, 0.0, 1.0), 5.0);
}

// n - normal vector, v - vector to camera, l - vector to light
vec3 CookTorrenceBRDF(vec3 _n, vec3 _v, vec3 _l, vec3 _h, float _roughness, vec3 _specColor)
{
    float D = SpecDistribution(_n, _h, _roughness);
    vec3 F = FresnelSchlick(max(dot(_h, _v), 0.0f), _specColor);
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

vec3 AmbientColor(vec3 _albedo, vec3 _N, vec3 _V, vec3 _R, vec3 _specColor, float _roughness, float _metalness)
{
    vec3 ambient = vec3(0.0f);

    float NDotV = max(dot(_N, _V), 0.0f);
    vec3 F = FresnelSchlickRoughness(NDotV, _specColor, _roughness);
    vec3 kSpec = F;
    vec3 kDiff  = 1.0f - kSpec;
    kDiff *= 1.0f - _metalness;

    vec3 irradiance = texture(irradianceMap, _N).rgb;
    vec3 diffuse = irradiance * _albedo;

    // Split-Sum approximation
    vec3 prefilteredColor = textureLod(prefilterMap, _R, _roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBrdf = texture(brdfMap, vec2(NDotV, 0.0f), _roughness).rg;
    vec3 specular = prefilteredColor * (F * envBrdf.x + envBrdf.y);

    ambient = kDiff * diffuse * specular;

    return ambient;
}

void main()
{
    vec3 lighting;
    vec3 lightDir;

    vec3 albedo = texture(albedoTex, UV).rgb;
    albedo = pow(albedo, vec3(2.2));
    vec3 normalTex = texture(normalMap, UV).rgb;
    float roughness = texture(roughnessMap, UV).r;
    float metalness = texture(metalnessMap, UV).r;

    vec3 nonMetal = vec3(NON_METAL_COLOR);
    vec3 f0 = mix(nonMetal, albedo, metalness);

    vec3 N = NormalsFromMap(normalTex);
    vec3 V = normalize(cameraPos - WorldPos);
    vec3 R = reflect(-V, N);

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 L;

        if(lights[i].type == LIGHT_TYPE_POINT)
        {
            
            L = normalize(lights[i].position - WorldPos);
            
        }
        else if(lights[i].type == LIGHT_TYPE_DIRECTIONAL)
        {
            L = normalize(-lights[i].direction);
        }
        
        vec3 H = normalize(V + L);

        vec3 specular = CookTorrenceBRDF(N, V, L, H, roughness, f0);
        vec3 diffuse = vec3(1.0) - FresnelSchlick(max(dot(H, V), 0.0f), f0);
        diffuse *= 1.0 - metalness;
        float NDotL = max(dot(N, L), 0.0);

        if(lights[i].type == LIGHT_TYPE_POINT)
        {
            
            lighting += (diffuse * albedo / PI + specular) * NDotL * lights[i].color * Attenuate(lights[i]);
            
        }
        else if(lights[i].type == LIGHT_TYPE_DIRECTIONAL)
        {
            lighting += (diffuse * albedo / PI + specular) * lights[i].color * NDotL;
        }
        
    }

    vec3 ambient = AmbientColor(albedo, N, V, R, f0, roughness, metalness);

    vec3 color = ambient + lighting;
    color = color / (color + vec3(1.0));

    vec3 gamma = pow(color, vec3(1.0f / 2.2f));
    FragColor = vec4(gamma, 1.0f);
}