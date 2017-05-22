#version 330 core
in vec3 ourColor;
in vec3 curPosition;

out vec4 color;

void main()
{
    color = vec4(curPosition, 1.0f);
}
