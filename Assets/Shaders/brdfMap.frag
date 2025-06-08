#version 330 core

out vec2 FragColor;

in vec2 UV;

uniform samplerCube skybox;
uniform float roughness;

const float PI = 3.14159265359f;
const uint SAMPLE_COUNT = 1024u;

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

float GeometricShadowing(vec3 _n, vec3 _v, float _roughness)
{
    float k = (_roughness * _roughness) / 2.0;
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

vec2 BRDF(float _NDotV, float _roughness)
{
    vec3 V;
    V.x = sqrt(1.0f - _NDotV * _NDotV);
    V.y = 0.0f;
    V.z = _NDotV;

    float A = 0.0f;
    float B = 0.0f;
    vec3 N = vec3(0.0f, 0.0f, 1.0);

     for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0f * dot(V, H) * H - V);

        float NDotL = max(L.z, 0.0f);
        float NDotH = max(H.z, 0.0f);
        float VDotH = max(dot(V, H), 0.0f);

        if(NDotL > 0.0f)
        {
            float G = GeometrySmith(N, V, L, _roughness);
            float GVis = (G * VDotH) / (NDotH * _NDotV);
            float Fc = pow(1.0f - VDotH, 5.0f);

            A += (1.0f - Fc) * GVis;
            B += Fc * GVis;
        }
    }

    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);

    return vec2(A, B);
}

void main()
{
    vec2 integratedBrdf = BRDF(UV.x, UV.y);
    FragColor = integratedBrdf;
}