#version 330 core
in vec2 TexCoords;
out vec4 outColor;

uniform vec4 color;
uniform sampler2D text;

void main()
{

  outColor = texture(text,TexCoords).r*color;

}
