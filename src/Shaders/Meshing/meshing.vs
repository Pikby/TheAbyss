#version 430 core

in vec3 inValue;
out ivec3 blockPos;

void main()
{

  blockPos = ivec3(inValue);
}
