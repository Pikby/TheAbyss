
#version 330 core
in vec2 vs_texCoords;
out vec4 fs_color;

uniform vec4 color;
uniform sampler2D text;
uniform int quadStyle;


const int DefaultStyle = 0;
const int RoundedEdgeStyle = 1;
const int InvertedEdgeStyle = 2;
const int CircleStyle = 3;
const int OutlineStyle = 4;
void main()
{


  switch(quadStyle)
  {
    case(DefaultStyle):
    {
      fs_color = color;
    } break;
    case(RoundedEdgeStyle):
    {
      const float radius = 0.05;
      float p1 = length(vs_texCoords - vec2(radius,radius));
      float p2 = length(vs_texCoords - vec2(1-radius,radius));
      float p3 = length(vs_texCoords - vec2(radius,1-radius));
      float p4 = length(vs_texCoords - vec2(1-radius,1-radius));


      float dist = p1 + p2 + p3 + p4;

      //Smooth the alpha between some magic numbers, probably some math behind this
      //TODO figure out that math
      float alpha = 1-smoothstep(3.1,3.15,dist);
      fs_color = vec4(color.rgb,color.a*alpha);
    }break;
    case(InvertedEdgeStyle):
    {
      const float radius = 0.1;
      float p1 = length(vs_texCoords - vec2(0,0));
      float p2 = length(vs_texCoords - vec2(1,0));
      float p3 = length(vs_texCoords - vec2(0,1));
      float p4 = length(vs_texCoords - vec2(1,1));

      float alpha = 1;
      if(p1 < radius || p2 < radius || p3 < radius || p4 < radius) alpha = 0;
      fs_color = vec4(color.rgb,color.a*alpha);
    }break;
    case(CircleStyle):
    {
      const float radius = 0.45;
      const float radius_squared = pow(radius,2);
      vec2 shift = vs_texCoords - vec2(0.5);
      float len = dot(shift,shift);
      float alpha = 1-smoothstep(radius_squared,radius_squared+0.02,len);
      fs_color = vec4(color.rgb,color.a*alpha);
    }break;
    case(OutlineStyle):
    {
      const float lineSize = 0.01;
      float alpha = 0;
      if(vs_texCoords.x > 1-lineSize || vs_texCoords.y > 1-lineSize ||
         vs_texCoords.x < lineSize   || vs_texCoords.y < lineSize  )
       {
         alpha = 1;
       }
    fs_color = vec4(color.rgb,color.a*alpha);
    }break;


  }
}
