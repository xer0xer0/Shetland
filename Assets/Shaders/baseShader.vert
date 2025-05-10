#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;


out vec2 UV;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;

uniform mat4 world;
uniform mat3 worldInvTranspose;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    UV = vec2(aUV.x, aUV.y);
    WorldPos = vec3(world * vec4(aPos, 1.0));
    Normal = worldInvTranspose * aNormal;
    Tangent = aTangent;

    gl_Position = projection * view * vec4(WorldPos, 1.0);
}