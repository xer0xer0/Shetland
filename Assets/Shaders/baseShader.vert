#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;

out vec2 UV;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 wvp = projection * view * world;
    gl_Position = wvp * vec4(aPos, 1.0);

    UV = vec2(aUV.x, aUV.y);

    vec3 T = normalize(vec3(world * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(world * vec4(aNormal, 0.0)));

    // Gram-Schmidt
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    // transpose to account for model scaling
    TBN = transpose(mat3(T, B, N));
    FragPos = TBN * vec3(world * vec4(aPos, 1.0));
}