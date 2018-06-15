#include <stdlib.h>
#include <atomic>
#include <sstream>
using std::stringstream;
#include <fstream>
using std::ifstream;

#include "bnTextureResourceManager.h"

TextureResourceManager& TextureResourceManager::GetInstance() {
  static TextureResourceManager instance;
  return instance;
}

void TextureResourceManager::LoadAllTextures(std::atomic<int> &status) {
  TextureType textureType = static_cast<TextureType>(0);
  while (textureType != TEXTURE_TYPE_SIZE) {
    status++;

    // TODO: Catch failed resources and try again
    Texture* texture = nullptr;
    texture = LoadTextureFromFile(paths[static_cast<int>(textureType)]);
    if (texture) textures.insert(pair<TextureType, Texture*>(textureType, texture));
    textureType = (TextureType)(static_cast<int>(textureType) + 1);
  }
}

Texture* TextureResourceManager::LoadTextureFromFile(string _path) {
  Texture* texture = new Texture();
  if (!texture->loadFromFile(_path)) {

    Logger::GetMutex()->lock();
    Logger::Logf("Failed loading texture: %s\n", _path.c_str());
    Logger::GetMutex()->unlock();

  } else {

    Logger::GetMutex()->lock();
    Logger::Logf("Loaded texture: %s\n", _path.c_str());
    Logger::GetMutex()->unlock();

  }
  return texture;
}

Texture* TextureResourceManager::GetTexture(TextureType _ttype) {
  return textures.at(_ttype);
}

sf::IntRect TextureResourceManager::GetCardRectFromID(unsigned ID) {
  return sf::IntRect((ID % 11) * 56, (ID / 11) * 48, 56, 48);
}

sf::IntRect TextureResourceManager::GetIconRectFromID(unsigned ID) {
  sf::IntRect result  = sf::IntRect(1+((ID % 20) * 16), 1+((ID / 20) * 16), 14, 14);

  // Start mapping icons to cards
  return result;
}

Font* TextureResourceManager::LoadFontFromFile(string _path) {
  Font* font = new Font();
  if (!font->loadFromFile(_path)) {
    Logger::Logf("Failed loading font: %s\n", _path.c_str());
  } else {
    Logger::Logf("Loaded font: %s\n", _path.c_str());
  }
  return font;
}

TextureResourceManager::TextureResourceManager(void) {
  //-Tiles-
  //Blue tile
  paths.push_back("resources/tiles/img_blue_normal.png");
  paths.push_back("resources/tiles/img_blue_cracked.png");
  paths.push_back("resources/tiles/img_blue_broken.png");
  paths.push_back("resources/tiles/img_blue_ice.png");
  paths.push_back("resources/tiles/img_blue_grass.png");
  paths.push_back("resources/tiles/img_blue_purple.png");
  paths.push_back("resources/tiles/img_blue_empty.png");
  //Red tile
  paths.push_back("resources/tiles/img_red_normal.png");
  paths.push_back("resources/tiles/img_red_cracked.png");
  paths.push_back("resources/tiles/img_red_broken.png");
  paths.push_back("resources/tiles/img_red_ice.png");
  paths.push_back("resources/tiles/img_red_grass.png");
  paths.push_back("resources/tiles/img_red_purple.png");
  paths.push_back("resources/tiles/img_red_empty.png");

  //-Navis-
  //Megaman
  paths.push_back("resources/navis/megaman/navi_megaman_atlas.png");

  //Starman
  paths.push_back("resources/navis/starman/navi_starman_atlas.png");

  //-Mobs-
  paths.push_back("resources/mobs/mob_move.png");
  paths.push_back("resources/mobs/mob_explosion.png");
  //Mettaur
  paths.push_back("resources/mobs/mettaur/mob_mettaur_move.png");
  paths.push_back("resources/mobs/mettaur/mob_mettaur_hit.png");
  paths.push_back("resources/mobs/mettaur/mob_mettaur_attack.png");

  // ProgsMan
  paths.push_back("resources/mobs/progsman/mob_progsman_idle.png");
  paths.push_back("resources/mobs/progsman/mob_progsman_move.png");
  paths.push_back("resources/mobs/progsman/mob_progsman_shoot.png");
  paths.push_back("resources/mobs/progsman/mob_progsman_hit.png");
  paths.push_back("resources/mobs/progsman/mob_progsman_punch.png");
  paths.push_back("resources/mobs/progsman/mob_progsman_throw.png");

  // Select screen "Anything Goes"
  paths.push_back("resources/mobs/select/random.png");

  //-Spells-
  //Bullet
  paths.push_back("resources/spells/spell_bullet_hit.png");
  paths.push_back("resources/spells/spell_buster_charge.png");
  //paths.push_back("resources/spells/spell_charged_bullet_hit.png");
  paths.push_back("resources/spells/spell_wave.png");
  paths.push_back("resources/spells/spell_prog_bomb.png");

  // Chip Select 
  paths.push_back("resources/ui/chip_select.png");
  paths.push_back("resources/ui/chip_cursor_small.png");
  paths.push_back("resources/ui/chip_cursor_big.png");
  paths.push_back("resources/ui/chip_cards.png");
  paths.push_back("resources/ui/chip_icons.png");
  paths.push_back("resources/ui/chip_nodata.png");
  paths.push_back("resources/ui/chip_senddata.png");

  // In battle signs
  paths.push_back("resources/ui/program_advance.png");

  // Battle Results Frame
  paths.push_back("resources/ui/battle_results.png");
  paths.push_back("resources/ui/zenny.png");
  paths.push_back("resources/ui/nodata.png");

  // Background/foreground
  paths.push_back("resources/backgrounds/title/bg_blue.png");
  paths.push_back("resources/backgrounds/title/prog-pulse.png");
  paths.push_back("resources/backgrounds/game_over/game_over.png");
  paths.push_back("resources/backgrounds/select/battle_select.png");
  paths.push_back("resources/backgrounds/main_menu/overlay.png");
  paths.push_back("resources/backgrounds/main_menu/ow.png");

  // other icons
  paths.push_back("resources/ui/gamepad_support_icon.png");
  paths.push_back("resources/ui/main_menu_ui.png");

  // other misc
  paths.push_back("resources/shaders/textures/distortion.png");
  paths.push_back("resources/shaders/textures/heat.png");
  paths.push_back("resources/shaders/textures/noise.png");
  paths.push_back("resources/shaders/textures/noise_invert.png");
}

TextureResourceManager::~TextureResourceManager(void) {
  for (auto it = textures.begin(); it != textures.end(); ++it) {
    delete it->second;
  }
}