#include <ft2build.h>
#include FT_FREETYPE_H

#include "../../headers/shaders.h"

#define TEXTFONTSIZE 64
struct Character {
    glm::vec2 size;    // Size of glyph
    glm::vec2 bearing;  // Offset from baseline to left/top of glyph
    uint advance;    // Horizontal offset to advance to next glyph
    float xstart;
    float ystart;
};



//Contains the 6 vertices that create the letter
struct CharacterVertex
{
  glm::vec4 vertices[6];
};
struct Point
{
	int dx, dy;
	int distSq() const { return dx*dx + dy*dy; }
};

struct Grid
{
	Point** grid;
  int rows;
  Grid(int y, int x) : rows(y)
  {
    grid = new Point*[y];
    for(int i=0;i<y;i++)
    {
      grid[i] = new Point[x];
    }
  }
  ~Grid()
  {
    for(int i=0;i<rows;i++)
    {
      delete grid[i];
    }
    delete grid;
  }
};


enum TextAlignment{TEXTALILEFT,TEXTALIRIGHT,TEXTALICENTER};

class TextRenderer
{
private:
  uint VAO=0, VBO=0;
  uint textAtlas;
  uint atlasWidth,atlastHeight;
  float textBuffer;
  float textGamma;
  int padding;
  glm::vec2 atlasDimensions;
  std::map<char, Character> characters;
  std::vector<CharacterVertex> characterVertices;

  //Functions and code from http://www.codersnotes.com/notes/signed-distance-fields/
  //After creating the atlas for the glyphs call genereateSDF to create the grid
  Point get( Grid &g, int x, int y );
  void put( Grid &g, int x, int y, const Point &p );
  void compare( Grid &g, Point &p, int x, int y, int offsetx, int offsety );
  void generateSDF( Grid &g );
  void loadTextAtlas(const FT_Face &face, int fontSize);
public:
  Shader GUIShaderText;
  void init();
  void drawAllText();
  void renderText(const std::string &text,const glm::vec2 &screenPos, float scale,
                  const glm::vec4 &color,const glm::mat3 &rot = glm::mat3(1),
                  TextAlignment alignment = TEXTALILEFT,int cursorPosition = -1);
  glm::vec3 calculateStringDimensions(const std::string& line,double scale);
  std::vector<std::string> splitString(const std::string& string,double scale, int viewLength);
};
