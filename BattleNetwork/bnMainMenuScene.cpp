#include <time.h>
#include "bnMainMenuScene.h"
#include "bnFolderScene.h"
#include "bnOverworldMap.h"
#include "bnInfiniteMap.h"
#include "bnSelectMobScene.h"
#include "bnMemory.h"
#include "bnCamera.h"
#include "bnInputManager.h"
#include "bnAudioResourceManager.h"
#include "bnShaderResourceManager.h"
#include "bnTextureResourceManager.h"
#include "bnEngine.h"
#include "bnAnimation.h"
#include "bnLanBackground.h"
#include <SFML/Graphics.hpp>
using sf::RenderWindow;
using sf::VideoMode;
using sf::Clock;
using sf::Event;
using sf::Font;
enum class SelectedNavi {
  STARMAN,
  MEGAMAN,
  SIZE // denotes length of list and if we've reached the end
};

int MainMenuScene::Run()
{
  Camera camera(ENGINE.GetDefaultView());

  // Selection input delays
  double maxSelectInputCooldown = 1000.0f / 2.f; // half of a second
  double selectInputCooldown = maxSelectInputCooldown;        

  // ui sprite maps
  sf::Sprite ui(*TEXTURES.GetTexture(TextureType::MAIN_MENU_UI));
  ui.setScale(2.f, 2.f);
  Animation uiAnimator("resources/ui/main_menu_ui.animation");
  uiAnimator.Load();

  // Stream menu music 
  AUDIO.Stream("resources/loops/loop_navi_customizer.ogg", true);

  // Transition
  sf::Shader& transition = *SHADERS.GetShader(ShaderType::TRANSITION);
  transition.setUniform("texture", sf::Shader::CurrentTexture);
  transition.setUniform("map", *TEXTURES.GetTexture(TextureType::NOISE_TEXTURE));
  transition.setUniform("progress", 0.f);
  float transitionProgress = 0.9f;
  ENGINE.RevokeShader();

  Clock clock;
  float elapsed = 0.0f;
  float totalTime = 0.f;
  int menuSelectionIndex = 0;

  sf::Sprite overlay(*TEXTURES.GetTexture(TextureType::MAIN_MENU));
  overlay.setScale(2.f, 2.f);

  sf::Sprite ow(*TEXTURES.GetTexture(TextureType::MAIN_MENU_OW));
  ow.setScale(2.f, 2.f);

  Background* bg = new LanBackground();

  Overworld::Map* map = new Overworld::InfiniteMap(10, 40, 47, 24);
  map->SetCamera(&camera);

  // Keep track of selected navi

  SelectedNavi currentNavi = SelectedNavi::MEGAMAN;
  sf::Sprite owNavi(*TEXTURES.GetTexture(TextureType::NAVI_MEGAMAN_ATLAS));
  owNavi.setScale(2.f, 2.f);
  owNavi.setPosition(0, -26.f);
  Animation naviAnimator("resources/navis/megaman/megaman.animation");
  naviAnimator.Load();
  naviAnimator.SetAnimation("PLAYER_OW_RD");
  naviAnimator << Animate::Mode(Animate::Mode::Loop);

  map->AddSprite(&owNavi);

  bool gotoNextScene = false;

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
    map->Update();

    ENGINE.Clear();
    ENGINE.SetView(camera.GetView());

    camera.Update(elapsed);

    bg->Draw();
    ENGINE.Draw(map);

    // Draw navi moving
    naviAnimator.Update(elapsed, &owNavi);

    int lastMenuSelectionIndex = menuSelectionIndex;

    // Scene keyboard controls 
    
    if (!gotoNextScene && transitionProgress == 0.f) {
      if (INPUT.has(PRESSED_UP)) {
        selectInputCooldown -= elapsed;

        if (selectInputCooldown <= 0) {
          // Go to previous mob 
          selectInputCooldown = maxSelectInputCooldown;
          menuSelectionIndex--;
        }
      }
      else if (INPUT.has(PRESSED_DOWN)) {
        selectInputCooldown -= elapsed;

        if (selectInputCooldown <= 0) {
          // Go to next mob 
          selectInputCooldown = maxSelectInputCooldown;
          menuSelectionIndex++;
        }
      }
      else {
        selectInputCooldown = 0;
      }

      owNavi.setPosition(owNavi.getPosition() + sf::Vector2f(0.5, 0));

      // camera.PlaceCamera(map->ScreenToWorld(owNavi.getPosition()-(ENGINE.GetDefaultView().getSize()/32.0f)));

      
      if (INPUT.has(PRESSED_ACTION1)) {

        // Folder Select
        if (menuSelectionIndex == 1) {
          gotoNextScene = true;
          AUDIO.Play(AudioType::CHIP_DESC);
        }

        // Navi select
        if (menuSelectionIndex == 2) {
          int nextNavi = (int)currentNavi + 1;
          currentNavi = (SelectedNavi)nextNavi;

          if (currentNavi == SelectedNavi::SIZE) {
            currentNavi = SelectedNavi::STARMAN;
          }

          if (currentNavi == SelectedNavi::MEGAMAN) {
  
            owNavi.setTexture(*TEXTURES.GetTexture(TextureType::NAVI_MEGAMAN_ATLAS));
            naviAnimator = Animation("resources/navis/megaman/megaman.animation");
            naviAnimator.Load();
            naviAnimator.SetAnimation("PLAYER_OW_RD");
            naviAnimator << Animate::Mode(Animate::Mode::Loop);
          }
          else if (currentNavi == SelectedNavi::STARMAN) {
            owNavi.setTexture(*TEXTURES.GetTexture(TextureType::NAVI_STARMAN_ATLAS));
            naviAnimator = Animation("resources/navis/starman/starman.animation");
            naviAnimator.Load();
            naviAnimator.SetAnimation("PLAYER_OW_RD");
            naviAnimator << Animate::Mode(Animate::Mode::Loop);
          }
        }

        // Mob select
        if (menuSelectionIndex == 3) {
          gotoNextScene = true;
          AUDIO.Play(AudioType::CHIP_DESC);
        }
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

      if (menuSelectionIndex == 1) {
        int result = FolderScene::Run();

        // reset internal clock (or everything will teleport)
        elapsed = static_cast<float>(clock.getElapsedTime().asMilliseconds());
        std::cout << "time slept: " << elapsed << "\n";
        clock.restart();
        elapsed = 0;

        if (result == 0) {
          break; // Breaks the while-loop
        }
      } else if (menuSelectionIndex == 3) {
        int result = SelectMobScene::Run();

        // reset internal clock (or everything will teleport)
        elapsed = static_cast<float>(clock.getElapsedTime().asMilliseconds());
        std::cout << "time slept: " << elapsed << "\n";
        clock.restart();
        elapsed = 0;

        if (result == 0) {
          break; // Breaks the while-loop
        }
      } 

      gotoNextScene = false;
    }

    menuSelectionIndex = std::max(0, menuSelectionIndex);
    menuSelectionIndex = std::min(3, menuSelectionIndex);

    
    if (menuSelectionIndex != lastMenuSelectionIndex) {
      AUDIO.Play(AudioType::CHIP_SELECT, 1);
    }

    ENGINE.Draw(overlay);

    uiAnimator.SetAnimation("CHIP_FOLDER");

    if (menuSelectionIndex == 0) {
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(50.f, 50.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("CHIP_FOLDER_LABEL");
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(100.f, 50.f);
      ENGINE.Draw(ui);
    }
    else {
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(20.f, 50.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("CHIP_FOLDER_LABEL");
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(100.f, 50.f);
      ENGINE.Draw(ui);
    }

    uiAnimator.SetAnimation("LIBRARY");

    if (menuSelectionIndex == 1) {
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(50.f, 120.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("LIBRARY_LABEL");
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(100.f, 120.f);
      ENGINE.Draw(ui);
    }
    else {
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(20.f, 120.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("LIBRARY_LABEL");
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(100.f, 120.f);
      ENGINE.Draw(ui);
    }

    uiAnimator.SetAnimation("NAVI");

    if (menuSelectionIndex == 2) {
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(50.f, 190.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("NAVI_LABEL");
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(100.f, 190.f);
      ENGINE.Draw(ui);
    }
    else {
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(20.f, 190.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("NAVI_LABEL");
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(100.f, 190.f);
      ENGINE.Draw(ui);
    }

    uiAnimator.SetAnimation("MOB_SELECT");

    if (menuSelectionIndex == 3) {
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(50.f, 260.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("MOB_SELECT_LABEL");
      uiAnimator.SetFrame(2, &ui);
      ui.setPosition(100.f, 260.f);
      ENGINE.Draw(ui);
    }
    else {
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(20.f, 260.f);
      ENGINE.Draw(ui);

      uiAnimator.SetAnimation("MOB_SELECT_LABEL");
      uiAnimator.SetFrame(1, &ui);
      ui.setPosition(100.f, 260.f);
      ENGINE.Draw(ui);
    }

    ENGINE.Draw(ui); 

    sf::Texture postprocessing = ENGINE.GetPostProcessingBuffer().getTexture(); // Make a copy
    sf::Sprite transitionPost;
    transitionPost.setTexture(postprocessing);

    transition.setUniform("progress", transitionProgress);

    LayeredDrawable* bake = new LayeredDrawable(transitionPost);
    bake->SetShader(&transition);

    ENGINE.Draw(bake);
    delete bake;

    // Write contents to screen (always last step)
    ENGINE.Display();

    elapsed = static_cast<float>(clock.getElapsedTime().asMilliseconds());
  }

  AUDIO.StopStream();
  ENGINE.RevokeShader();

  return 0; // signal game over to the main stack
}