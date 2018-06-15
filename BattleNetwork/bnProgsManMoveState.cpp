#pragma once
#include "bnProgsManMoveState.h"
#include "bnProgsMan.h"
#include "bnTile.h"
#include "bnField.h"
#include "bnProgsManIdleState.h"
#include "bnProgsManPunchState.h"

#include "bnMettaur.h"

ProgsManMoveState::ProgsManMoveState() : isMoving(false), AIState<ProgsMan>() { ; }
ProgsManMoveState::~ProgsManMoveState() { ; }

void ProgsManMoveState::OnEnter(ProgsMan& progs) {
}

void ProgsManMoveState::OnUpdate(float _elapsed, ProgsMan& progs) {
  if (isMoving) return; // We're already moving (animations take time)


  Tile* temp = progs.tile;
  Tile* next = nullptr;

  int random = rand() % 50;

  if (random > 15) {
    // Hunt the player
    Entity* target = progs.GetTarget();

    if (target && target->GetTile()) {
      if (target->GetTile()->GetY() < progs.GetTile()->GetY()) {
        nextDirection = Direction::UP;
      }
      else if (target->GetTile()->GetY() > progs.GetTile()->GetY()) {
        nextDirection = Direction::DOWN;
      }
      else {
        if (target->GetTile()->GetX() == progs.GetTile()->GetX() - 1) {
          // Punch
          return progs.StateChange<ProgsManPunchState>();
        }
        else {
          // Try shooting. 

          // Throw bombs.
        }
      }
    }
  }


  // otherwise aimlessly move around 
  int randDirection = rand() % 4;
  nextDirection = static_cast<Direction>(randDirection);

  if (nextDirection == Direction::UP) {
    if (progs.tile->GetY() - 1 > 0) {
      next = progs.field->GetAt(progs.tile->GetX(), progs.tile->GetY() - 1);
      if (progs.Teammate(next->GetTeam()) && next->IsWalkable())
        if (!next->ContainsEntityType<ProgsMan>() && !next->ContainsEntityType<Mettaur>()) {
          progs.SetTile(next);
        }
        else {
          next = nullptr;
          nextDirection = Direction::DOWN;
        }
      else
        next = nullptr;
    }
    else {
      nextDirection = Direction::DOWN;
    }
  }
  else if (nextDirection == Direction::LEFT) {
    if (progs.tile->GetX() - 1 > 0) {
      next = progs.field->GetAt(progs.tile->GetX() - 1, progs.tile->GetY());
      if (progs.Teammate(next->GetTeam()) && next->IsWalkable())
        if (!next->ContainsEntityType<ProgsMan>() && !next->ContainsEntityType<Mettaur>()) {
          progs.SetTile(next);
        }
        else {
          next = nullptr;
          nextDirection = Direction::RIGHT;
        }
      else
        next = nullptr;
    }
  }
  else if (nextDirection == Direction::DOWN) {
    if (progs.tile->GetY() + 1 <= (int)progs.field->GetHeight()) {
      next = progs.field->GetAt(progs.tile->GetX(), progs.tile->GetY() + 1);
      if (progs.Teammate(next->GetTeam()) && next->IsWalkable())
        if (!next->ContainsEntityType<ProgsMan>() && !next->ContainsEntityType<Mettaur>()) {
          progs.SetTile(next);
        }
        else {
          next = nullptr;
          nextDirection = Direction::UP;
        }
      else
        next = nullptr;
    }
    else {
      nextDirection = Direction::UP;
    }
  }
  else if (nextDirection == Direction::RIGHT) {
    if (progs.tile->GetX() + 1 <= (int)progs.field->GetWidth()) {
      next = progs.field->GetAt(progs.tile->GetX() + 1, progs.tile->GetY());
      if (progs.Teammate(next->GetTeam()) && next->IsWalkable())
        if (!next->ContainsEntityType<ProgsMan>() && !next->ContainsEntityType<Mettaur>()) {
          progs.SetTile(next);
        }
        else {
          next = nullptr;
          nextDirection = Direction::LEFT;
        }
      else
        next = nullptr;
    }
  }


  if (next) {
    progs.tile->AddEntity((Entity*)&progs);
    temp->RemoveEntity((Entity*)&progs);
    auto onFinish = [&progs]() { progs.StateChange<ProgsManIdleState>(); };
    progs.SetAnimation(MOB_MOVING, onFinish);
    isMoving = true;
  }
  else {
    progs.StateChange<ProgsManIdleState>();
  }
}

void ProgsManMoveState::OnLeave(ProgsMan& progs) {

}

