#include "bnProgsManPunchState.h"
#include "bnProgsMan.h"

ProgsManPunchState::ProgsManPunchState() : AIState<ProgsMan>()
{
}


ProgsManPunchState::~ProgsManPunchState()
{
}

void ProgsManPunchState::OnEnter(ProgsMan& progs) {
  auto onFinish = [this, &progs]() { this->Attack(progs); };
  progs.SetAnimation(MOB_ATTACKING, onFinish);
}

void ProgsManPunchState::OnLeave(ProgsMan& progs) {
}

void ProgsManPunchState::OnUpdate(float _elapsed, ProgsMan& progs) {

}

void ProgsManPunchState::Attack(ProgsMan& progs) {
  Battle::Tile* tile = progs.GetTile();
  if (tile->GetX() - 1 >= 1) {

    Battle::Tile* next = 0;
    next = progs.GetField()->GetAt(tile->GetX() - 1, tile->GetY());

    Entity* entity = 0;

    while (next->GetNextEntity(entity)) {
      Player* isPlayer = dynamic_cast<Player*>(entity);

      if (isPlayer && !isPlayer->IsPassthrough()) {
        isPlayer->Move(Direction::LEFT);
        isPlayer->AdoptNextTile();
        isPlayer->Hit(20);
      }
    }
  }

  tile = 0;

  progs.StateChange<ProgsManIdleState>();
}