#include <time.h>
#include "bnSelectMobScene.h"
#include "bnTile.h"
#include "bnField.h"
#include "bnMob.h"
#include "bnMemory.h"
#include "bnCamera.h"
#include "bnInputManager.h"
#include "bnAudioResourceManager.h"
#include "bnShaderResourceManager.h"
#include "bnTextureResourceManager.h"
#include "bnEngine.h"
#include "bnBattleScene.h"
#include "bnMobFactory.h"
#include "bnRandomMettaurMob.h"
#include "bnProgsManBossFight.h"
#include "bnTwoMettaurMob.h"
#include "bnCanodumbMob.h"

#include <SFML/Graphics.hpp>
using sf::RenderWindow;
using sf::VideoMode;
using sf::Clock;
using sf::Event;
using sf::Font;

int SelectMobScene::Run()
{
  Camera camera(ENGINE.GetDefaultView());

  // Menu name font
  sf::Font* font = TEXTURES.LoadFontFromFile("resources/fonts/dr_cain_terminal.ttf");
  sf::Text* menuLabel = new sf::Text("BATTLE SELECT", *font);
  menuLabel->setCharacterSize(15);
  menuLabel->setPosition(sf::Vector2f(20.f, 5.0f));

  // Selection input delays
  double maxSelectInputCooldown = 1000.0f / 2.f; // half of a second
  double selectInputCooldown = maxSelectInputCooldown;

  // MOB UI font
  sf::Font *mobFont = TEXTURES.LoadFontFromFile("resources/fonts/mmbnthick_regular.ttf");
  sf::Text *mobLabel = new sf::Text("Mettaur", *mobFont);
  mobLabel->setPosition(275.f, 15.f);

  sf::Text *attackLabel = new sf::Text("1", *mobFont);
  attackLabel->setPosition(325.f, 30.f);

  sf::Text *speedLabel = new sf::Text("1", *mobFont);
  speedLabel->setPosition(325.f, 45.f);

  sf::Font *hpFont = TEXTURES.LoadFontFromFile("resources/fonts/mgm_nbr_pheelbert.ttf");
  sf::Text *hpLabel = new sf::Text("20", *hpFont);
  hpLabel->setOutlineColor(sf::Color(48, 56, 80));
  hpLabel->setOutlineThickness(2.f);
  hpLabel->setScale(0.8f, 0.8f);
  hpLabel->setOrigin(hpLabel->getLocalBounds().width, 0);
  hpLabel->setPosition(sf::Vector2f(180.f, 33.0f));

  float maxNumberCooldown = 500.f;
  float numberCooldown = maxNumberCooldown; // half a second

  // select menu graphic
  sf::Sprite bg(*TEXTURES.GetTexture(TextureType::BATTLE_SELECT_BG));
  bg.setScale(2.f, 2.f);

  // Current mob graphic
  sf::Sprite mob(*TEXTURES.GetTexture(TextureType::MOB_METTAUR_IDLE));
  mob.setScale(2.f, 2.f);
  mob.setOrigin(mob.getLocalBounds().width / 2.f, mob.getLocalBounds().height / 2.f);
  mob.setPosition(110.f, 130.f);

  // Animator for mobs
  Animation mobAnimator;

  // Distortion effect
  sf::Texture& distortionMap = *TEXTURES.GetTexture(TextureType::DISTORTION_TEXTURE);

  // It is important to set repeated to true to enable scrolling upwards
  distortionMap.setRepeated(true);

  // Transition
  sf::Shader& transition = *SHADERS.GetShader(ShaderType::TRANSITION);
  transition.setUniform("texture", sf::Shader::CurrentTexture);
  transition.setUniform("map", *TEXTURES.GetTexture(TextureType::NOISE_TEXTURE));
  transition.setUniform("progress", 0.f);
  float transitionProgress = 0.9f;
  ENGINE.RevokeShader();

  bool gotoNextScene = false;

  sf::Shader& shader = *SHADERS.GetShader(ShaderType::DISTORTION);

  shader.setUniform("currentTexture", sf::Shader::CurrentTexture);
  shader.setUniform("distortionMapTexture", distortionMap);

  // Current selection index
  int mobSelectionIndex = 0;

  Clock clock;
  float elapsed = 0.0f;
  float totalTime = 0.f;
  while (ENGINE.Running()) {
    
    float elapsedSeconds = clock.restart().asSeconds();
    totalTime += elapsedSeconds;

    float FPS = 0.f;

    if (elapsedSeconds > 0.f) {
      FPS = 1.0f / elapsedSeconds;
      std::string fpsStr = std::to_string(FPS);
      fpsStr.resize(4);
      ENGINE.GetWindow()->setTitle(sf::String(std::string("FPS: ") + fpsStr));
    }
  
    INPUT.update();

    ENGINE.Clear();
    //ENGINE.SetView(camera.GetView());

    camera.Update(elapsed);

    ENGINE.Draw(bg);
    ENGINE.Draw(menuLabel);

    // Draw mob name with shadow
    mobLabel->setPosition(277.f, 17.f);
    mobLabel->setFillColor(sf::Color(138, 138, 138));
    ENGINE.Draw(mobLabel);
    mobLabel->setPosition(275.f, 15.f);
    mobLabel->setFillColor(sf::Color::White);
    ENGINE.Draw(mobLabel);

    // Draw attack rating with shadow
    attackLabel->setPosition(432.f, 48.f);
    attackLabel->setFillColor(sf::Color(88, 88, 88));
    ENGINE.Draw(attackLabel);
    attackLabel->setPosition(430.f, 46.f);
    attackLabel->setFillColor(sf::Color::White);
    ENGINE.Draw(attackLabel);

    // Draw speed rating with shadow
    speedLabel->setPosition(432.f, 80.f);
    speedLabel->setFillColor(sf::Color(88, 88, 88));
    ENGINE.Draw(speedLabel);
    speedLabel->setPosition(430.f, 78.f);
    speedLabel->setFillColor(sf::Color::White);
    ENGINE.Draw(speedLabel);

    // Draw hp
    ENGINE.Draw(hpLabel);

    ENGINE.DrawUnderlay();
    ENGINE.DrawLayers();
    ENGINE.DrawOverlay();

    // Scene keyboard controls
    if (!gotoNextScene && transitionProgress == 0.f) {
      if (INPUT.has(PRESSED_LEFT)) {
        selectInputCooldown -= elapsed;

        if (selectInputCooldown <= 0) {
          // Go to previous mob 
          selectInputCooldown = maxSelectInputCooldown;
          mobSelectionIndex--;

          // Number scramble effect
          numberCooldown = maxNumberCooldown;
        }
      }
      else if (INPUT.has(PRESSED_RIGHT)) {
        selectInputCooldown -= elapsed;

        if (selectInputCooldown <= 0) {
          // Go to next mob 
          selectInputCooldown = maxSelectInputCooldown;
          mobSelectionIndex++;

          // Number scramble effect
          numberCooldown = maxNumberCooldown;
        }
      }
      else {
        selectInputCooldown = 0;
      }

      if (INPUT.has(PRESSED_ACTION2)) {
        gotoNextScene = true;
        AUDIO.Play(AudioType::CHIP_DESC_CLOSE);
      }
    }

    if (elapsed > 0) {
      if (gotoNextScene) {
        transitionProgress += 0.1f / elapsed;
      }
      else {
        transitionProgress -= 0.1f / elapsed;
      }
    }

    transitionProgress = std::max(0.f, transitionProgress);
    transitionProgress = std::min(1.f, transitionProgress);

    if (transitionProgress >= 1.f) {
      return 2;

      gotoNextScene = false;
    }

    mobSelectionIndex = std::max(0, mobSelectionIndex);
    mobSelectionIndex = std::min(3, mobSelectionIndex);

    if (mobSelectionIndex == 0) {
      mob.setTexture(*TEXTURES.GetTexture(TextureType::MOB_METTAUR_IDLE),true);
      mob.setPosition(110.f, 130.f);
      mobLabel->setString("Mettaur");
      speedLabel->setString("2");
      attackLabel->setString("1");
      hpLabel->setString("20");
    }
    else if (mobSelectionIndex == 1) {
      mob.setTexture(*TEXTURES.GetTexture(TextureType::MOB_PROGSMAN_IDLE),true);
      mob.setPosition(100.f, 110.f);
      mobLabel->setString("ProgsMan");
      speedLabel->setString("4");
      attackLabel->setString("3");
      hpLabel->setString("300");
    }
    else if (mobSelectionIndex == 2) {
      mob.setTexture(*TEXTURES.GetTexture(TextureType::MOB_CANODUMB_ATLAS));
      mob.setPosition(90.f, 130.f);

      mobAnimator = Animation("resources/mobs/canodumb/canodumb.animation");
      mobAnimator.Load();
      mobAnimator.SetAnimation(MOB_CANODUMB_IDLE_1);
      mobAnimator.SetFrame(1, &mob);

      mobLabel->setString("Canodumb");
      speedLabel->setString("1");
      attackLabel->setString("4");
      hpLabel->setString("60");
    }
    else {
      mob.setTexture(*TEXTURES.GetTexture(TextureType::MOB_ANYTHING_GOES),true);
      mob.setPosition(110.f, 110.f);
      mobLabel->setString("Random Mob");
      speedLabel->setString("*");
      attackLabel->setString("*");
      hpLabel->setString("");
    }

    if (numberCooldown > 0) {
      numberCooldown -= elapsed;
      std::string newstr;

      for (int i = 0; i < mobLabel->getString().getSize(); i++) {
        double progress = (maxNumberCooldown - numberCooldown) / maxNumberCooldown;
        double index = progress * mobLabel->getString().getSize();

        if (i < (int)index) {
          newstr += mobLabel->getString()[i];
        } else {
          if (mobLabel->getString()[i] != ' ') {
            newstr += (char)(((rand() % (90 - 65)) + 65) + 1);
            AUDIO.Play(AudioType::TEXT, 0);
          }
          else {
            newstr += ' ';
          }
        }
      }

      int randAttack = rand() % 10;
      int randSpeed = rand() % 10;

      attackLabel->setString(std::to_string(randAttack));
      speedLabel->setString(std::to_string(randSpeed));
      mobLabel->setString(sf::String(newstr));
    }
             
    float progress = (maxNumberCooldown - numberCooldown) / maxNumberCooldown;

    if (progress > 1.f) progress = 1.f;

    mob.setColor(sf::Color(255, 255, 255, (sf::Uint32)(255.0*progress)));

    shader.setUniform("time", 1.f-progress);
    shader.setUniform("distortionFactor", 2.f*(1.f-progress));
    shader.setUniform("riseFactor", 0.2f);


    // Refresh mob graphic origin every frame as it may change
    mob.setOrigin(mob.getTextureRect().width / 2.f, mob.getTextureRect().height / 2.f);
    hpLabel->setOrigin(hpLabel->getLocalBounds().width, 0);

    LayeredDrawable* bake = new LayeredDrawable(sf::Sprite(mob));
    bake->SetShader(&shader);

    ENGINE.Draw(bake);
    delete bake;

    // Make a selection
    if (INPUT.has(PRESSED_ACTION1)) {
      AUDIO.Play(AudioType::CHIP_CONFIRM, 0);

      // Stop music and go to battle screen 
      AUDIO.StopStream();

      Field* field(new Field(6, 3));
      MobFactory* factory = nullptr;

      if (mobSelectionIndex == 0) {
        // see how the random mob works around holes
        field->GetAt((rand()) % 3 + 4, (rand() % 3) + 1)->SetState(TileState::EMPTY);
        factory = new TwoMettaurMob(field);
      }
      else if (mobSelectionIndex == 1) {
        factory = new ProgsManBossFight(field);
      }
      else if (mobSelectionIndex == 2) {
        factory = new CanodumbMob(field);
      }
      else {
        factory = new RandomMettaurMob(field);
      }

      Mob* mob = factory->Build();

      int win = BattleScene::Run(mob);

      delete mob;
      delete factory;
      delete field;

      if (win != 1) {
        break; // end while-loop
      }

      // Re-play music
      AUDIO.Stream("resources/loops/loop_navi_customizer.ogg", true);
    }

    sf::Texture postprocessing = ENGINE.GetPostProcessingBuffer().getTexture(); // Make a copy
    sf::Sprite transitionPost;
    transitionPost.setTexture(postprocessing);

    transition.setUniform("progress", transitionProgress);

    bake = new LayeredDrawable(transitionPost);
    bake->SetShader(&transition);

    ENGINE.Draw(bake);
    delete bake;

    // Write contents to screen (always last step)
    ENGINE.Display();

    elapsed = static_cast<float>(clock.getElapsedTime().asMilliseconds());
  }
  delete font;
  delete mobFont;
  delete hpFont;
  delete mobLabel;
  delete attackLabel;
  delete speedLabel;
  delete menuLabel;
  delete hpLabel;

  ENGINE.RevokeShader();

  return 0; // signal game over to the main stack
}