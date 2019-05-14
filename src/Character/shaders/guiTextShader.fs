#version 330 core
in vec2 vs_texCoords;
in vec4 vs_color;
in float vs_charScale;

out vec4 fs_color;

uniform sampler2D text;


void main()
{

  //Values are determined through testing, feel free to experiment for better resolution
  float buf = 0.8;

  //Branch should be optimized by the compiler, no need to optimize
  float gamma = vs_charScale <= 1 ? 0.05 : 0.2;


  float dist = texture(text, vs_texCoords).r;
  float alpha = smoothstep(buf-gamma,buf+gamma,dist);

  vec4 sampled = vec4(vs_color.rgb, vs_color.a*alpha);
  fs_color = sampled;

}
