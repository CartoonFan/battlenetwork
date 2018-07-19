#pragma once

#include <SFML/Graphics.hpp>
#include "bnTextureResourceManager.h"
#include "bnAudioResourceManager.h"
#include "bnChipFolder.h"
#include "bnEngine.h"

class ChipSelectionCust {
public:
  struct Bucket {
    Chip* data;
    short state;
  };

private:
  sf::Sprite custSprite;
  sf::Sprite cursorSmall; // TODO: make these two animate
  sf::Sprite cursorBig;
  LayeredDrawable icon;
  LayeredDrawable chipCard;
  LayeredDrawable chipNoData;
  LayeredDrawable chipSendData;
  LayeredDrawable element;
  sf::Shader& greyscale;
  sf::Font* labelFont;
  sf::Font* codeFont;
  sf::Text smCodeLabel;
  sf::Text label;
  int chipCount;
  int selectCount;
  int chipCap;
  int cursorPos;
  int cursorRow;
  bool areChipsReady;
  ChipFolder* folder;
  Chip** selectedChips;
  Bucket* queue;
  Bucket** selectQueue;
public:
  ChipSelectionCust(ChipFolder* _folder, int);
  ~ChipSelectionCust();

  // GUI ops
  bool CursorUp();
  bool CursorDown();
  bool CursorRight();
  bool CursorLeft();
  bool CursorAction();
  bool CursorCancel();

  bool IsOutOfView();
  bool IsInView();
  void Move(sf::Vector2f delta);
  void Draw();

  // Chip ops
  void GetNextChips();
  Chip** GetChips();
  void ClearChips();
  const int GetChipCount();
  void ResetState();
  bool AreChipsReady();
};

