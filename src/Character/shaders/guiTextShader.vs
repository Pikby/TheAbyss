#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, float package ,float color>
out vec2 vs_texCoords;
out vec4 vs_color;
out float vs_charScale;


uniform mat4 projection;
struct Character
{
    vec2 size;    // Size of glyph
    vec2 bearing;  // Offset from baseline to left/top of glyph
    uint advance;    // Horizontal offset to advance to next glyph
    float xstart;
    float ystart;
};
uniform Character characters[128];

void main()
{
  int package = floatBitsToInt(vertex.z);
  vec2 tex = vec2( (package & 2)>>1,package & 1);
  int c = (package >> 8) & 0xff;
  int scale = ((package >> 16) & 0xff) - 128;

  int colorPacked = floatBitsToInt(vertex.w);

  Character character = characters[c];
  gl_Position =  projection*vec4(vertex.xy, -0.5, 1.0);
  float x = character.xstart + tex.x*(character.size.x);
  float y = character.ystart + tex.y*(character.size.y);

  vs_texCoords = vec2(x,y);
  vs_charScale = pow(2,scale);
  vs_color = vec4((colorPacked & 0xff)/255.0f,
                    ((colorPacked >> 8) & 0xff)/255.0f,
                    ((colorPacked >> 16) & 0xff)/255.0f,
                    ((colorPacked >> 24) & 0xff)/255.0f);
}
