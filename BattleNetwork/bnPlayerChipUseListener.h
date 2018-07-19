#pragma once

#include "bnAudioResourceManager.h"
#include "bnChipUseListener.h"
#include "bnPlayer.h"
#include "bnTile.h"
#include "bnCannon.h"
#include "bnBasicSword.h"

class PlayerChipUseListener : public ChipUseListener {
private:
  Player * player;
  sf::Clock invisTimer;

public:
  PlayerChipUseListener(Player* _player) : ChipUseListener() { player = _player; }
  PlayerChipUseListener(Player& _player) : ChipUseListener() { player = &_player;  }

  void OnChipUse(Chip& chip) {
    std::string name = chip.GetShortName();

    if (name.substr(0, 5) == "Recov") {
      player->SetHealth(player->GetHealth() + chip.GetDamage());
      AUDIO.Play(AudioType::RECOVER);

      auto onFinish = [this]() { this->player->SetAnimation(PLAYER_IDLE);  };

      player->SetAnimation(PLAYER_HEAL, onFinish);

    }
    else if (name == "CrckPanel") {
      Tile* top = player->GetField()->GetAt(player->GetTile()->GetX() + 1, 1);
      Tile* mid = player->GetField()->GetAt(player->GetTile()->GetX() + 1, 2);
      Tile* low = player->GetField()->GetAt(player->GetTile()->GetX() + 1, 3);

      if (top) { top->SetState(TileState::CRACKED); }
      if (mid) { mid->SetState(TileState::CRACKED); }
      if (low) { low->SetState(TileState::CRACKED); }

      AUDIO.Play(AudioType::PANEL_CRACK);
    }
    else if (name == "Invis") {

      // TODO: Move chip use logic out of chip UI. Utilize component.s
      /*if (invisTimer.getElapsedTime() > sf::seconds(10)) {
        player->SetPassthrough(false);
        player->setColor(sf::Color(255, 255, 255, 255));
      }*/

      // Todo make this a time-based component
      AUDIO.Play(AudioType::INVISIBLE);
      player->SetPassthrough(true);
      player->setColor(sf::Color(255, 255, 255, (sf::Uint8)(255 / 2.f)));
      invisTimer.restart();
    }
    else if (name == "XtrmeCnnon") {
      Cannon* xtreme1 = new Cannon(player->GetField(), player->GetTeam(), chip.GetDamage());
      Cannon* xtreme2 = new Cannon(player->GetField(), player->GetTeam(), chip.GetDamage());
      Cannon* xtreme3 = new Cannon(player->GetField(), player->GetTeam(), chip.GetDamage());


      ENGINE.GetCamera().ShakeCamera(25, sf::seconds(1));

      auto onFinish = [this]() { this->player->SetAnimation(PLAYER_IDLE);  };
      player->SetAnimation(PLAYER_CANNON, onFinish);
      AUDIO.Play(AudioType::CANNON);

      xtreme1->SetDirection(Direction::RIGHT);
      xtreme2->SetDirection(Direction::RIGHT);
      xtreme3->SetDirection(Direction::RIGHT);

      player->GetField()->OwnEntity(xtreme1, 4, 1);
      player->GetField()->OwnEntity(xtreme2, 4, 2);
      player->GetField()->OwnEntity(xtreme3, 4, 3);
    }
    else if (name == "Cannon") {
      Cannon* cannon = new Cannon(player->GetField(), player->GetTeam(), chip.GetDamage());
      auto onFinish = [this]() { this->player->SetAnimation(PLAYER_IDLE);  };
      player->SetAnimation(PLAYER_CANNON, onFinish);
      AUDIO.Play(AudioType::CANNON);

      cannon->SetDirection(Direction::RIGHT);

      player->GetField()->OwnEntity(cannon, player->GetTile()->GetX() + 1, player->GetTile()->GetY());
    }
    else if (name == "Swrd") {
      auto onFinish = [this]() { this->player->SetAnimation(PLAYER_IDLE);  };

      player->SetAnimation(PLAYER_SLASHING, onFinish);

      BasicSword* sword = new BasicSword(player->GetField(), player->GetTeam(), chip.GetDamage());

      AUDIO.Play(AudioType::SWORD_SWING);

      player->GetField()->OwnEntity(sword, player->GetTile()->GetX() + 1, player->GetTile()->GetY());
    }
    else if (name == "LongSwrd") {
      auto onFinish = [this]() { this->player->SetAnimation(PLAYER_IDLE);  };

      player->SetAnimation(PLAYER_SLASHING, onFinish);

      BasicSword* sword = new BasicSword(player->GetField(), player->GetTeam(), chip.GetDamage());
      BasicSword* sword2 = new BasicSword(player->GetField(), player->GetTeam(), chip.GetDamage());

      AUDIO.Play(AudioType::SWORD_SWING);

      if (player->GetField()->GetAt(player->GetTile()->GetX() + 1, player->GetTile()->GetY())) {
        player->GetField()->OwnEntity(sword, player->GetTile()->GetX() + 1, player->GetTile()->GetY());
      }

      if (player->GetField()->GetAt(player->GetTile()->GetX() + 2, player->GetTile()->GetY())) {
        player->GetField()->OwnEntity(sword2, player->GetTile()->GetX() + 2, player->GetTile()->GetY());
      }
    }
    else if (name == "WideSwrd") {
      auto onFinish = [this]() { this->player->SetAnimation(PLAYER_IDLE);  };

      player->SetAnimation(PLAYER_SLASHING, onFinish);

      BasicSword* sword = new BasicSword(player->GetField(), player->GetTeam(), chip.GetDamage());
      BasicSword* sword2 = new BasicSword(player->GetField(), player->GetTeam(), chip.GetDamage());

      AUDIO.Play(AudioType::SWORD_SWING);

      if (player->GetField()->GetAt(player->GetTile()->GetX() + 1, player->GetTile()->GetY())) {
        player->GetField()->OwnEntity(sword, player->GetTile()->GetX() + 1, player->GetTile()->GetY());
      }

      if (player->GetField()->GetAt(player->GetTile()->GetX() + 1, player->GetTile()->GetY() + 1)) {
        player->GetField()->OwnEntity(sword2, player->GetTile()->GetX() + 1, player->GetTile()->GetY() + 1);
      }
    }
  }
};