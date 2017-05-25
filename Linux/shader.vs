#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
uniform vec3 newPosition;

out vec3 ourColor;
out vec3 curPosition;
out vec2 TexCoord;

void main()
{
    curPosition = position + newPosition;
    gl_Position = vec4(curPosition, 1.0f);
    ourColor = color;
    TexCoord = texCoord;
}
