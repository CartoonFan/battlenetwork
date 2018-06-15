#include <random>
#include <time.h>

#include "bnCannon.h"
#include "bnTile.h"
#include "bnField.h"
#include "bnPlayer.h"
#include "bnProgsMan.h"
#include "bnMettaur.h"
#include "bnTextureResourceManager.h"
#include "bnAudioResourceManager.h"

#define COOLDOWN 40.0f
#define DAMAGE_COOLDOWN 50.0f

#define BULLET_ANIMATION_SPRITES 3
#define BULLET_ANIMATION_WIDTH 30
#define BULLET_ANIMATION_HEIGHT 27

Cannon::Cannon(Field* _field, Team _team, int _damage) {
  field = _field;
  team = _team;
  direction = Direction::NONE;
  deleted = false;
  hit = false;
  progress = 0.0f;
  hitHeight = 0.0f;
  srand((unsigned int)time(nullptr));
  random = rand() % 20 - 20;

  damage = _damage;
  //TODO: make new sprite animation for charged bullet
  texture = TEXTURES.GetTexture(TextureType::SPELL_BULLET_HIT);
 
  setScale(2.f, 2.f);
  for (int x = 0; x < BULLET_ANIMATION_SPRITES; x++) {
    animation.addFrame(0.3f, IntRect(BULLET_ANIMATION_WIDTH*x, 0, BULLET_ANIMATION_WIDTH, BULLET_ANIMATION_HEIGHT));
  }
}

Cannon::~Cannon(void) {
}

void Cannon::Update(float _elapsed) {
  if (hit) {
    if (progress == 0.0f) {
      setTexture(*texture);
      setPosition(tile->getPosition().x + tile->GetWidth() / 2.f + random, tile->getPosition().y + tile->GetHeight() / 2.f - hitHeight);
    }
    progress += 0.2f;
    animation(*this, fmin(progress, 1.0f));
    if (progress >= 1.f) {
      deleted = true;
    }
    return;
  }

  tile->AffectEntities(this);

  cooldown += _elapsed;
  if (cooldown >= COOLDOWN) {
    Move(direction);
    cooldown = 0;
  }
}

bool Cannon::Move(Direction _direction) {
  tile->RemoveEntity(this);
  Tile* next = nullptr;
  if (_direction == Direction::UP) {
    if (tile->GetY() - 1 > 0) {
      next = field->GetAt(tile->GetX(), tile->GetY() - 1);
      SetTile(next);
    }
  }
  else if (_direction == Direction::LEFT) {
    if (tile->GetX() - 1 > 0) {
      next = field->GetAt(tile->GetX() - 1, tile->GetY());
      SetTile(next);
    }
    else {
      deleted = true;
      return false;
    }
  }
  else if (_direction == Direction::DOWN) {
    if (tile->GetY() + 1 <= (int)field->GetHeight()) {
      next = field->GetAt(tile->GetX(), tile->GetY() + 1);
      SetTile(next);
    }
  }
  else if (_direction == Direction::RIGHT) {
    if (tile->GetX() < (int)field->GetWidth()) {
      next = field->GetAt(tile->GetX() + 1, tile->GetY());
      SetTile(next);
    }
    else {
      deleted = true;
      return false;
    }
  }
  tile->AddEntity(this);
  return true;
}

void Cannon::Attack(Entity* _entity) {
  Mettaur* isMob = dynamic_cast<Mettaur*>(_entity);
  if (isMob) {
    isMob->Hit(damage);
    hitHeight = isMob->GetHitHeight();
    hit = true;

    if (isMob->IsCountered()) {
      AUDIO.Play(AudioType::COUNTER, 0);
      isMob->Stun(1000);

      if (isMob->GetHealth() == 0) {
        // Slide entity back a few pixels
        isMob->setPosition(isMob->getPosition().x + 50.f, isMob->getPosition().y);
      }
    }

  }
  else {
    ProgsMan* isProgs = dynamic_cast<ProgsMan*>(_entity);
    if (isProgs) {
      isProgs->Hit(damage);
      hitHeight = isProgs->GetHitHeight();
      hit = true;
    }
  }

  if (hit) {
   //  AUDIO.Play(AudioType::HURT, 0);
  }
}

vector<Drawable*> Cannon::GetMiscComponents() {
  return vector<Drawable*>();
}

void Cannon::AddAnimation(int _state, FrameAnimation _animation, float _duration) {
  //animator.addAnimation(static_cast<Buster>(_state), _animation, sf::seconds(_duration));
  assert(false && "Buster does not have an animator");
}