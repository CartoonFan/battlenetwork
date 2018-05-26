#pragma once
#include <vector>
using std::vector;
#include <iostream>
using std::cout;
using std::endl;

#include "bnTile.h"
#include "bnEntity.h"
#include "bnPlayer.h"
#include "bnMemory.h"

class Field {
public:
  Field(void);
  Field(int _width, int _height);
  ~Field(void);

  int GetWidth() const;
  int GetHeight() const;

  bool GetNextTile(Tile*& out);

  void AddEntity(Entity* _entity, int x, int y);
  void OwnEntity(Entity* _entity, int x, int y);
  void RemoveEntity(Entity* _entity);
  Tile* FindEntity(Entity* _entity) const;
  bool GetNextEntity(Entity*& out, int _depth) const;

  void SetAt(int _x, int _y, Team _team);
  Tile* GetAt(int _x, int _y) const;

  void Update(float _elapsed);

private:
  int width;
  int height;
  vector<vector<Tile*>> tiles;
  vector<Entity*> entities;
};