#version 330 core

out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube skybox;
uniform float roughness;

const float PI = 3.14159265359f;
const uint SAMPLE_COUNT = 1024u;
const float IMG_RESOLUTION = 512.0f;

float RadicalInverseVdC(uint _bits)
{
    _bits = (_bits << 16u) | (_bits >> 16u);
    _bits = ((_bits & 0x55555555u) << 1u) | ((_bits & 0xAAAAAAAAu) >> 1u);
    _bits = ((_bits & 0x33333333u) << 2u) | ((_bits & 0xCCCCCCCCu) >> 2u);
    _bits = ((_bits & 0x0F0F0F0Fu) << 4u) | ((_bits & 0xF0F0F0F0u) >> 4u);
    _bits = ((_bits & 0x00FF00FFu) << 8u) | ((_bits & 0xFF00FF00u) >> 8u);
    
    return float(_bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint _i, uint _N)
{
    return vec2(float(_i)/float(_N), RadicalInverseVdC(_i));
}

vec3 ImportanceSampleGGX(vec2 _Xi, vec3 _N, float _roughness)
{
    float a = _roughness * _roughness;
    float phi = 2.0f * PI * _Xi.x;
    float cosT = sqrt((1.0f - _Xi.y) / (1.0f + (a * a - 1.0f) * _Xi.y));
    float sinT = sqrt(1.0f - cosT * cosT);

    // convert spherical coordinates to cartesian
    vec3 h;
    h.x = cos(phi) * sinT;
    h.y = sin(phi) * sinT;
    h.z = cosT;

    // convert tangent space to world space
    vec3 up = abs(_N.z) < 0.999 ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, _N));
    vec3 bitangent = cross(_N, tangent);

    vec3 sampleVec = tangent * h.x + bitangent * h.y + _N * h.z;
    
    return normalize(sampleVec);
}

float DistributionGGX(vec3 _N, vec3 _H, float _roughness)
{
    float a = _roughness * _roughness;
    float a2 = a * a;
    float NDotH = max(dot(_N, _H), 0.0f);
    float NDotH2 = NDotH * NDotH;

    float denom = NDotH2 * (a2 - 1.0f) + 1.0f;
    denom = PI * denom * denom;

    return a2 / denom;
}

void main()
{
    vec3 N = normalize(WorldPos);
    float totalWeight = 0.0f;

    vec3 R = N;
    vec3 V = R;

    vec3 prefilteredColor = vec3(0.0f);
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0f * dot(V, H) * H - V);

        float NDotL = max(dot(N, L), 0.0f);
        if(NDotL > 0.0f)
        {
            // sample the mip level based on pdf and roughness
            float D = DistributionGGX(N, H, roughness);
            float NDotH = max(dot(N, H), 0.0f);
            float HDotV = max(dot(H, V), 0.0f);
            float pdf = D * NDotH / (4.0f * HDotV) + 0.0001f;

            float texel = 4.0f * PI / (6.0f * IMG_RESOLUTION * IMG_RESOLUTION);
            float sample = 1.0f / (float(SAMPLE_COUNT) * pdf + 0.0001f);
            float mipLevel = roughness == 0.0f ? 0.0f : 0.5f * log2(sample / texel);

            prefilteredColor += textureLod(skybox, L, mipLevel).rgb * NDotL;
            totalWeight += NDotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0f);
}