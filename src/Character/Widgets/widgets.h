#ifndef WIDGETLIBRARY
#define WIDGETLIBRARY



#include <glm/glm.hpp>
#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <functional>
#include "texttypes.h"


class Widget
{
protected:
  bool focusable=true;
  bool focused=false;
  glm::vec2 origin=glm::vec2(0);
  glm::vec2 dimensions=glm::vec2(0);
  double padding = 0.01;
  double size;
  glm::vec4 color = glm::vec4(1);
  std::function<void(Widget* parent)> onDrawFunction = NULL;
public:
  Widget(){}
  Widget(glm::vec2 Origin,glm::vec2 Dimensions = glm::vec2(0)): origin(Origin),dimensions(Dimensions){}
  virtual bool isIn(const glm::vec2 pos)
  {
    glm::vec2 bottomLeft = origin-glm::vec2(padding);
    glm::vec2 topRight = origin+dimensions+glm::vec2(padding);
    bool horzIn = pos.x > bottomLeft.x && pos.x < topRight.x;
    bool vertIn = pos.y > bottomLeft.y && pos.y < topRight.y;
    return horzIn && vertIn;
  }
  void setOrigin(const glm::vec2 &pos){origin = pos;}
  glm::vec2 getOrigin(){return origin;}

  glm::vec2 getDimensions(){return dimensions;}

  virtual void setColor(glm::vec4 Color){color = Color;}
  virtual bool getFocusable(){return focusable;}
  virtual void setFocusable(bool b){focusable = b;};
  virtual void setFocused(bool b){focused = b;}
  virtual void draw(){};
  virtual void submitEvent(){};
  virtual void handleClick(){};
  virtual void handleKeyInput(int key,int action){}
  virtual void handleCharInput(uint character){}
  virtual void handleScrollInput(double xoffset,double yoffset){}
  virtual void handleMouseHover(const glm::vec2& mousePos){};
  virtual void handleMouseInput(int button,int action){};
  virtual void handleMouseMovement(double xpos, double ypos){};
  virtual void onDraw(){};
  virtual void setOnDraw(std::function<void(Widget*)> func){onDrawFunction = func;}

};

class EditBox : public Widget
{
private:
  std::string defaultText;
  std::string text = "";
  double characterScale;
  int cursorPosition=0;
  std::function<void(std::string)> submitHandler = NULL;
public:
  EditBox(){};
  EditBox(const std::string& text,const glm::vec2& Origin,const glm::vec2& dims,double CharacterScale,std::function<void(std::string)> submitHandler = NULL);
  std::string getText(){return text;}
  void updateText(const std::string &str){text = str;cursorPosition=0;}
  void draw() override;
  void handleCharInput(uint character) override;
  void handleKeyInput(int key,int action) override;
};


class Quad : public Widget
{
private:
  QuadDrawType type = DEFAULTQUAD;
  uint textId;
public:
  Quad(){};
  Quad(const glm::vec2& Origin, const glm::vec2& Dims, uint id = -1);
  void draw() override;
  void setDrawType(QuadDrawType newType) {type = newType;}
};

class Label : public Widget
{
private:
  glm::mat3 rotMat = glm::mat3(1);
  std::string label;
  double characterScale;
  TextAlignment  textAlignment;
public:
  Label(){};
  Label(const std::string& text,const glm::vec2& Origin,double CharacterScale,TextAlignment ali= TEXTALILEFT);
  void updateLabel(const std::string& str);
  void draw() override;
  void setRotMat(const glm::mat3& mat){rotMat = mat;}
};

class Button : public Widget
{
private:
  double textScale;
  double pressedTime;

  glm::vec4 textColor = glm::vec4(0,0,0,1);
  std::string text = "";
  std::function<void(int)> clickHandler = NULL;
public:
  Button(const std::string& Text,const glm::vec2& Origin, const glm::vec2& Dimensions,std::function<void(int)> ClickHandler = std::function<void(int)>(NULL));
  ~Button(){};

  void draw() override;
  void handleMouseInput(int button, int action) override;
  void submitEvent() override;
  void setTextColor(glm::vec4 color){textColor = color;}
};

class ChatBox : public Widget
{
private:
  unsigned char lineLength=32;
  double characterScale=0.5f;
  double extendTime;
  int cursorPosition=0;
  std::string currentLine;
  std::list<std::string> history;
  std::list<std::string>::iterator focusTarget;
  EditBox inputBox;
public:
  ChatBox(const glm::vec2& origin,const glm::vec2& dimensions,double size);
  std::string getInputLine(){return currentLine;}
  void extendFor(double seconds);
  void updateInputLine(const std::string &line){currentLine = line;}
  void addLineToHistory(const std::string &line);

  void setFocused(bool b){focused = b;inputBox.setFocused(b);}
  void handleCharInput(uint character) override;
  void handleKeyInput(int key,int action) override;
  void handleScrollInput(double xoffset,double yoffset) override;
  void draw() override;
};

enum CellType{SQUAREIMAGECELL,ROUNDIMAGECELL,TEXTIMAGECELL};


class ImageGrid : public Widget
{
private:
  int rows;
  int columns;
  glm::vec2 cellDimensions;
  glm::vec2 cellOffset;
  uint* textureArray;
  uint getCellTexture(int x,int y){return textureArray[x+y*columns];}
public:
  ImageGrid(glm::vec2 Origin,int Rows, int Cols,glm::vec2 CellDimensions, glm::vec2 CellOffset);

  void setImage(int id,uint texture){textureArray[id] = texture;}
  void handleMouseHover(const glm::vec2& mousePos);

  bool isIn(const glm::vec2 pos) override;
  void draw() override;
};


class WidgetList : public Widget
{
private:
  int selected = -1;
  std::list<Widget*> widgetList;
  glm::vec2 cellOffset;
  std::function<glm::vec2(Widget* parent,int i)> offsetFunction = NULL;
public:
  WidgetList(const glm::vec2 &Origin);
  void draw() override;
  void handleKeyInput(int key,int action) override;

  void addWidget(Widget* widget);
  void setOffsetFunction(std::function<glm::vec2(Widget* parent, int i)> f){offsetFunction =f;}
};


class InGame : public Widget
{
public:
  void draw() override;
  void handleKeyInput(int key,int action) override;
  void handleCharInput(uint character) override;
  void handleScrollInput(double xoffset,double yoffset) override;
  void handleMouseMovement(double xpos, double ypos) override;
  void handleMouseInput(int button,int action) override;
};


#endif
