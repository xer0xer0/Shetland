#version 330 core

out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube skybox;

const float PI = 3.14159265359f;
const float SAMPLE_STEP = 0.025f;

void main()
{		
    vec3 normal = normalize(WorldPos);
  
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float numSamples = 0.0f;
    float sinP, cosP, sinT, cosT;
    
    vec3 total = vec3(0.0f);

    for(float phi = 0; phi < 2.0 * PI; phi += SAMPLE_STEP)
    {
        sinP = sin(phi);
        cosP = cos(phi);

        for(float theta = 0; theta < 0.5f * PI; theta += SAMPLE_STEP)
        {
            sinT = sin(theta);
            cosT = cos(theta);
            
            vec3 hemisphereDir = vec3(sinT * cosP, sinT * sinP, cosT);
            // convert to world space
            vec3 sampleVec = hemisphereDir.x * right + hemisphereDir.y * up + hemisphereDir.z * normal;

            total += texture(skybox, sampleVec).rgb * cosT * sinT;
            numSamples++;

        }
    }

    total = PI * total * (1.0f / float(numSamples));
    FragColor = vec4(total, 1.0f);
  //  FragColor = texture(skybox, WorldPos);
}