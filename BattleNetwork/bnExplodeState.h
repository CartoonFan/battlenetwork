#pragma once
#include "bnMeta.h"
#include "bnEntity.h"
#include "bnAIState.h"
#include "bnLongExplosion.h"
#include "bnShaderResourceManager.h"

/*
  This state can be used by any Entity in the engine. 
  It uses constraints to ensure the type passed in Any 
  is a subclass of Entity. 

  This state spawns an explosion and flickers the 
  entity at it's current animation. Once the explosion
  is finished, the entity is tried for deletion. Since 
  this state is used when health < 0, the deletion will
  succeed.
*/
template<typename Any>
class ExplodeState : public AIState<Any>
{
private:
  Entity* explosion;
  sf::Shader* whiteout;

public:
  ExplodeState();
  ~ExplodeState();

  void OnEnter(Any& e);
  void OnUpdate(float _elapsed, Any& e);
  void OnLeave(Any& e);
};

#include "bnLongExplosion.h"
#include "bnField.h"
#include "bnLogger.h"

template<typename Any>
ExplodeState<Any>::ExplodeState() : AIState<Any>() {
  // Enforce template constraints on class
  _DerivedFrom<Any, Entity>();

  // If we make it here, we are the proper type
  explosion = nullptr;

  whiteout = SHADERS.GetShader(ShaderType::WHITE);
}

template<typename Any>
ExplodeState<Any>::~ExplodeState() {
  /* explosion artifact is deleted by field */
}

template<typename Any>
void ExplodeState<Any>::OnEnter(Any& e) {
  e.LockState(); // Lock AI state. This is a final state.
  e.SetPassthrough(true); // Shoot through dying enemies

  /* Spawn an explosion */
  Battle::Tile* tile = e.GetTile();
  Field* field = e.GetField();
  explosion = new LongExplosion(field, e.GetTeam());
  field->OwnEntity(explosion, tile->GetX(), tile->GetY());
}

template<typename Any>
void ExplodeState<Any>::OnUpdate(float _elapsed, Any& e) {
  /* freeze frame, flash white */
  if ((int)((_elapsed) * 5) % 2 == 0) {
    e.SetShader(whiteout);
  }
  else {
    e.SetShader(nullptr);
  }

  /* If explosion is over, delete the entity*/
  if (explosion->IsDeleted()) {
    e.TryDelete();
  }
}

template<typename Any>
void ExplodeState<Any>::OnLeave(Any& e) { }