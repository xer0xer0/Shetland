#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 wvp = projection * view * world;
    gl_Position = wvp * vec4(aPos, 1.0);

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    // transpose to account for model scaling
    Normal = mat3(transpose(inverse(world))) * aNormals;
    FragPos = vec3(world * vec4(aPos, 1.0));
}