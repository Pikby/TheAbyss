#pragma once
#include <list>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <mutex>
#include "../headers/perlinnoise.h"
#include "../headers/items.h"

#define CHUNKSIZE 32

//Class which holds the data for each individual chunk
class World;
class BSP
{
private:
  std::string worldName;
  std::mutex BSPMutex;
  //Opaque objects
  std::shared_ptr<std::vector<GLfloat>> oVertices;
  std::shared_ptr<std::vector<GLuint>> oIndices;
  GLuint oVBO, oEBO, oVAO;

  std::shared_ptr<std::vector<GLfloat>> oVerticesBuffer;
  std::shared_ptr<std::vector<GLuint>> oIndicesBuffer;
  //Translucent objects
  std::shared_ptr<std::vector<GLfloat>> tVertices;
  std::shared_ptr<std::vector<GLuint>> tIndices;

  std::shared_ptr<std::vector<GLfloat>> tVerticesBuffer;
  std::shared_ptr<std::vector<GLuint>> tIndicesBuffer;
  GLuint tVBO, tEBO, tVAO;

  int addVertex(int renderType, float x, float y, float z,float xn, float yn, float zn, float texX, float texY);
  void addIndices(int renderType,int index1, int index2, int index3, int index4);

  unsigned char worldMap[CHUNKSIZE*CHUNKSIZE*CHUNKSIZE];

public:
  int xCoord,yCoord,zCoord;
  bool toRender,toBuild,toDelete,isGenerated,inUse;

  std::shared_ptr<BSP>  nextNode,prevNode;
  std::shared_ptr<BSP>  leftChunk,rightChunk,frontChunk;
  std::shared_ptr<BSP>  backChunk,topChunk,bottomChunk;

  BSP(int x,int y,int z, std::string worldName);
  BSP(int x,int y,int z, std::string val, std::string worldName);
  BSP();
  ~BSP();
  void generateTerrain();
  void render();
  void addBlock(int x, int y, int z,char id);
  void freeGL();
  bool blockExists(int x,int y,int z);
  int blockVisibleType(int x, int y, int z);
  int getBlock(int x, int y, int z);
  void delBlock(int x, int y, int z);
  void saveChunk();
  std::string compressChunk();
  glm::vec3 offset(float x, float y,float z);
  void build();
  void drawOpaque();
  void drawTranslucent();

  void del();
  void disconnect();

};

/*
class BSPNode
{
  private:

  public:
  BSP curBSP;
  BSPNode(int x,int y,int z);
  BSPNode(int x,int y,int z,std::string val);
  ~BSPNode();
  void saveChunk();
  bool blockExists(int x, int y, int z);
  int blockVisibleType(int x, int y, int z);
  void build();
  void drawOpaque();
  void drawTranslucent();
  void generateTerrain();
  std::string getCompressedChunk();
  void delBlock(int x, int y, int z);
  void addBlock(int x, int y, int z, int id);
  void del();
  void disconnect();
  //next and prev node for the linked list of all nodes


  //references to the 6 cardinal neighbours of the chunk
  std::shared_ptr<BSPNode>  leftChunk;
  std::shared_ptr<BSPNode>  rightChunk;
  std::shared_ptr<BSPNode>  frontChunk;
  std::shared_ptr<BSPNode>  backChunk;
  std::shared_ptr<BSPNode>  topChunk;
  std::shared_ptr<BSPNode>  bottomChunk;

  //Flags for use inbetween pointers

};
*/
