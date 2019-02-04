#version 330 core

uniform sampler2D curTexture;
uniform sampler2D bloomTexture;
uniform float exposure;

in vec2 TexCoords;
out vec4 outColor;






void main()
{
  const float gamma = 1.1;
  vec3 hdrColor = texture(curTexture, TexCoords).rgb + texture(bloomTexture, TexCoords).rgb;


  vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
  // gamma correction
  mapped = pow(mapped, vec3(1.0 / gamma));



  outColor = vec4(mapped,1.0f);
}
