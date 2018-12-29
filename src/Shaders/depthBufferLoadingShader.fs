#version 330 core

uniform sampler2D depthTexture;
in vec2 TexCoords;

void main()
{
  gl_FragDepth = texture(depthTexture, TexCoords).r;
}
