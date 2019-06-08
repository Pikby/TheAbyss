

// Std. Includes
#include <iostream>
#include <map>
#include <string>
// GLEW
#include <algorithm>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include "textrenderer.h"
#include "../../headers/shaders.h"
#include "../include/gui.h"

const Point inside = { 0, 0 };
const Point empty = { 9999, 9999 };


Point TextRenderer::get( Grid &g, int x, int y )
{
	// OPTIMIZATION: you can skip the edge check code if you make your grid
	// have a 1-pixel gutter.
	if ( x >= 0 && y >= 0 && x < atlasDimensions.x && y < atlasDimensions.y )
	return g.grid[y][x];
	else
	return empty;
}

void TextRenderer::put( Grid &g, int x, int y, const Point &p )
{
	g.grid[y][x] = p;
}

void TextRenderer::compare( Grid &g, Point &p, int x, int y, int offsetx, int offsety )
{
	Point other = get( g, x+offsetx, y+offsety );
	other.dx += offsetx;
	other.dy += offsety;

	if (other.distSq() < p.distSq())
	p = other;
}

void TextRenderer::generateSDF( Grid &g )
{
	// Pass 0
	for (int y=0;y<atlasDimensions.y;y++)
	{
		for (int x=0;x<atlasDimensions.x;x++)
		{
			Point p = get( g, x, y );
			compare( g, p, x, y, -1,  0 );
			compare( g, p, x, y,  0, -1 );
			compare( g, p, x, y, -1, -1 );
			compare( g, p, x, y,  1, -1 );
			put( g, x, y, p );
		}

		for (int x=atlasDimensions.x-1;x>=0;x--)
		{
			Point p = get( g, x, y );
			compare( g, p, x, y, 1, 0 );
			put( g, x, y, p );
		}
	}

	// Pass 1
	for (int y=atlasDimensions.y-1;y>=0;y--)
	{
		for (int x=atlasDimensions.x-1;x>=0;x--)
		{
			Point p = get( g, x, y );
			compare( g, p, x, y,  1,  0 );
			compare( g, p, x, y,  0,  1 );
			compare( g, p, x, y, -1,  1 );
			compare( g, p, x, y,  1,  1 );
			put( g, x, y, p );
		}

		for (int x=0;x<atlasDimensions.x;x++)
		{
			Point p = get( g, x, y );
			compare( g, p, x, y, -1, 0 );
			put( g, x, y, p );
		}
	}
}



void TextRenderer::loadTextAtlas(const FT_Face &face, int fontSize)
{
	FT_Set_Pixel_Sizes(face,0,fontSize);

	int glyphSize = fontSize + fontSize/4;
	atlasDimensions =  13.0f*glm::vec2(glyphSize);
	char zeroImage[int(atlasDimensions.x*atlasDimensions.y)];
	memset(zeroImage,0,atlasDimensions.x*atlasDimensions.y);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasDimensions.x, atlasDimensions.y, 0, GL_RED, GL_UNSIGNED_BYTE, zeroImage);
	int x = fontSize;
	int y = fontSize;
	for(int i = 32; i < 128; i++)
	{
		if(FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		padding = glyphSize/3;
		Character character =
		{
			glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(unsigned int)face->glyph->advance.x,(x-padding/2)/atlasDimensions.x,(y-padding/2)/atlasDimensions.y

		};
		x += glyphSize;
		if(x>= atlasDimensions.x)
		{
			x = glyphSize;
			y += glyphSize;
		}

		characters.insert(std::pair<char, Character>(i, character));
		GUIShaderText->setVec2("characters[" + std::to_string(i) + "].size",(character.size+glm::vec2(padding))/atlasDimensions);
		GUIShaderText->setVec2("characters[" + std::to_string(i) + "].bearing",character.bearing/atlasDimensions);
		GUIShaderText->setUInt("characters[" + std::to_string(i) + "].advance",character.advance);
		GUIShaderText->setFloat("characters[" + std::to_string(i) + "].xstart",character.xstart);
		GUIShaderText->setFloat("characters[" + std::to_string(i) + "].ystart",character.ystart);

	}
}


void TextRenderer::init()
{
	GUIShaderText = std::make_unique<Shader>(Shader("GUIShaders/guiTextShader.vs","GUIShaders/guiTextShader.fs"));
	glm::mat4 projection = glm::ortho(0, GUI::dimensions.x, 0,  GUI::dimensions.y);
	GUIShaderText->use();
	GUIShaderText->setMat4("projection",projection);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	FT_Face face;
	if (FT_New_Face(ft, "../assets/fonts/arial.ttf", 0, &face))
	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;


	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&textAtlas);
	glBindTexture(GL_TEXTURE_2D, textAtlas);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	loadTextAtlas(face,64);

	std::cout <<"dims:" << atlasDimensions.x << ":" << atlasDimensions.y << "\n";

	unsigned char image[int((atlasDimensions.x)*atlasDimensions.y)];


	glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_UNSIGNED_BYTE,image);
	//saveTexture("atlas.bmp",atlasDimensions.x,atlasDimensions.y,image);
	Grid Grid1(atlasDimensions.y,atlasDimensions.x);
	Grid Grid2(atlasDimensions.y,atlasDimensions.x);



	for( int y=0;y<atlasDimensions.y;y++ )
	{
		for ( int x=0;x<atlasDimensions.x;x++ )
		{
			unsigned char val = image[int(x+y*atlasDimensions.x)];
			if(val < 128)
			{
				put( Grid1, x, y, inside );
				put( Grid2, x, y, empty );
			} else {
				put( Grid2, x, y, inside );
				put( Grid1, x, y, empty );
			}
		}
	}
	generateSDF(Grid1);
	generateSDF(Grid2);

	for( int y=0;y<atlasDimensions.y;y++ )
	{
		for ( int x=0;x<atlasDimensions.x;x++ )
		{
			int dist1 = (int)( sqrt( (double)get( Grid1, x, y ).distSq() ) );
			int dist2 = (int)( sqrt( (double)get( Grid2, x, y ).distSq() ) );
			int dist = dist1 - dist2;

			int c = (dist+11)*21;
			if ( c < 0 ) c = 0;
			if ( c > 255 ) c = 255;

			image[int(x+y*atlasDimensions.x)]=c;
		}


	}


	//Shrink the atlas
	int factor = 2;
	unsigned char newImage[int(atlasDimensions.x/factor*atlasDimensions.y/factor)];
	for(int x = 0;x<atlasDimensions.x/factor;x++ )
	{
		for(int y = 0;y<atlasDimensions.y/factor;y++)
		{
			int total = image[int((x)*factor+(y)*factor*atlasDimensions.x)] +image[int((x+1)*factor+(y)*factor*atlasDimensions.x)]
			+ image[int((x)*factor+(y+1)*factor*atlasDimensions.x)] +image[int((x+1)*factor+(y+1)*factor*atlasDimensions.x)];

			newImage[int(x+y*atlasDimensions.x/factor)] = total/4;
		}
	}



	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasDimensions.x/factor,atlasDimensions.y/factor, 0, GL_RED, GL_UNSIGNED_BYTE, newImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	//saveTexture("signedDistanceFieldsmall.bmp",atlasDimensions.x/factor,atlasDimensions.y/factor,newImage);

	glBindTexture(GL_TEXTURE_2D,0);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	//Vertex data is 4 floats,xpos,ypos,package of textinfo,and color,

	glBufferData(GL_ARRAY_BUFFER,characterVertices.size()*sizeof(float)*6*4,&(characterVertices.front()), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE,4*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}

void TextRenderer::renderText(const std::string &text,const glm::vec2 &screenPos, float scale,const glm::vec4 &color,const glm::mat3& rot,TextAlignment alignment,int cursorPosition)
{

	scale /= 64.0f;

	glm::vec3 resPos = glm::vec3(screenPos*glm::vec2(GUI::dimensions),1);
	uint8_t r = (int)(color.r*255);
	uint8_t g = (int)(color.g*255);
	uint8_t b = (int)(color.b*255);
	uint8_t a = (int)(color.a*255);
	uint32_t packedColor = r | (g << 8) | (b << 16) | (a << 24);

	float xAlignment;
	switch(alignment)
	{
		case(TEXTALILEFT):
		xAlignment = 0;
		break;
		case(TEXTALIRIGHT):
		xAlignment = calculateStringDimensions(text,scale).x;
		break;
		case(TEXTALICENTER):
		xAlignment = calculateStringDimensions(text,scale).x/2;
		break;
	}

	int characterCount = 0;
	for(auto c = text.begin(); c != text.end(); c++)
	{
		if(*c == '\n')
		{
			resPos.x = screenPos.x*GUI::dimensions.x;
			resPos.y -= scale*64.0f;
		}

		Character ch = characters[*c];

		float w = (ch.size.x+padding) * scale;
		float h = (ch.size.y+padding) * scale;

		glm::vec3 pos = glm::vec3(ch.bearing.x * scale,-(ch.size.y+padding - ch.bearing.y) * scale,1) - glm::vec3(xAlignment,0,0);
		glm::vec3 cursorPos = glm::vec3(ch.bearing.x * scale,-(ch.size.y - ch.bearing.y) * scale,1) - glm::vec3(xAlignment,0,0);

		glm::vec3 offset = glm::vec3(w,h,1);

		glm::vec2 bl = resPos + glm::vec3(pos.x,pos.y,1)*rot;
		glm::vec2 tl = resPos + glm::vec3(pos.x,pos.y+offset.y,1)*rot;
		glm::vec2 tr = resPos + glm::vec3(pos.x+offset.x,pos.y+offset.y,1)*rot;
		glm::vec2 br = resPos + glm::vec3(pos.x+offset.x,pos.y,1)*rot;

		auto toFloat = [](int a)
		{
			return *(float*)&a;
		};

		uint8_t byteScale = round(log2(scale))+128;
		uint32_t package = ((*c) << 8) | (byteScale << 18);
		float floatColor = toFloat(packedColor);
		CharacterVertex vert =
		{
			glm::vec4(bl,toFloat(01 | package),floatColor),
			glm::vec4(tl,toFloat(00 | package),floatColor),
			glm::vec4(br,toFloat(11 | package),floatColor),

			glm::vec4(tl,toFloat(00 | package),floatColor),
			glm::vec4(br,toFloat(11 | package),floatColor),
			glm::vec4(tr,toFloat(10 | package),floatColor),
		};
		characterVertices.push_back(vert);

		if(characterCount == cursorPosition)
		{
			glm::vec2 quadPos = glm::vec2(resPos+pos+glm::vec3(padding/4)*scale);
			quadPos = quadPos/(glm::vec2)GUI::dimensions;
			GUI::setQuadDepth(-1.0);
			GUI::drawQuad(quadPos,quadPos+glm::vec2(0.001,scale*(64.0f/GUI::dimensions.y)),glm::vec4(0,0,0,1));
			GUI::setQuadDepth(0.0);
		}
		glm::vec3 advance = glm::vec3((ch.advance >> 6) * scale,0,1)*rot;

		resPos.x += advance.x; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		resPos.y += advance.y;

		characterCount++;
	}
}

void TextRenderer::drawAllText()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textAtlas);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,characterVertices.size()*6*4*sizeof(float),&(characterVertices.front()), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE,4*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glViewport(0,0,GUI::dimensions.x,GUI::dimensions.y);
	GUIShaderText->use();
	glm::mat4 projection = glm::ortho(0.0f, (float)GUI::dimensions.x, 0.0f,  (float)GUI::dimensions.y);
	GUIShaderText->setMat4("projection",projection);
	glDepthMask(GL_FALSE);
	glDrawArrays(GL_TRIANGLES, 0, characterVertices.size()*6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	characterVertices.clear();

	static int lastSize = 10;
	characterVertices.reserve(lastSize);
	lastSize = characterVertices.size();

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

}

glm::vec3 TextRenderer::calculateStringDimensions(const std::string& line,double scale)
{
	double horizontalLength = 0;
	double offY = -std::numeric_limits<double>::max();
	double maxY = -std::numeric_limits<double>::max();

	for(auto itr = line.begin();itr!=line.end();itr++)
	{
		Character c = characters[*itr];
		horizontalLength += (c.advance>>6) * scale;
		double ypos = (c.size.y - c.bearing.y) * scale;
		offY = std::max(offY,ypos);
		maxY = std::max(maxY,ypos+c.size.y * scale);
	}
	return glm::vec3(horizontalLength,offY,maxY);
}

std::vector<std::string> TextRenderer::splitString(const std::string& string,double scale, int viewLength)
{
	std::vector<std::string> ret;
	std::string currentLine = "";
	double horizontalLength = 0;
	for(auto itr = string.begin(); itr != string.end();itr++)
	{
		char curChar = *itr;
		Character c = characters[curChar];
		horizontalLength += (c.advance>>6) * scale;

		if(horizontalLength > viewLength)
		{
			ret.push_back(currentLine);
			currentLine = "";
			horizontalLength = 0;
		}
		currentLine.push_back(curChar);

	}
	if(currentLine != "") ret.push_back(currentLine);
	return ret;
}
