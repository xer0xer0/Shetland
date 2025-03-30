#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aNormals;
layout (location = 3) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 wvp = projection * view * world;
    gl_Position = wvp * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}