#include "bnGridBackground.h"
#include "bnLogger.h"
#include "bnTextureResourceManager.h"
#include "bnEngine.h"

#define X_OFFSET 0
#define Y_OFFSET 0

#define COMPONENT_FRAME_COUNT 12
#define COMPONENT_WIDTH 240
#define COMPONENT_HEIGHT 160

GridBackground::GridBackground(void)
  : x(0.0f), y(0), progress(0.0f), Background(LOAD_TEXTURE(NAVI_SELECT_BG), 240, 160) {
  FillScreen(sf::Vector2u(COMPONENT_WIDTH, COMPONENT_HEIGHT));
}

GridBackground::~GridBackground(void) {
}

void GridBackground::Update(float _elapsed) {
  progress += 1.0f * _elapsed;
  if (progress >= 1.f) progress = 0.0f;

  int frame = (int)(progress * COMPONENT_FRAME_COUNT);

  TextureOffset(sf::Vector2f((float)(frame*COMPONENT_WIDTH), 0));

}