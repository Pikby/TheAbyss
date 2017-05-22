#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
uniform vec3 newPosition;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(position.x +newPosition.x,position.y+newPosition.y,position.z+newPosition.z, 1.0f);
    ourColor = color;
}
