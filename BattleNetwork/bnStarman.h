#pragma once

#include "bnPlayer.h"
#include "bnPlayerState.h"
#include "bnTextureType.h"
#include "bnPlayerHealthUI.h"
#include "bnChargeComponent.h"
#include "bnAnimationComponent.h"
#include "bnAI.h"
#include "bnPlayerControlledState.h"
#include "bnPlayerIdleState.h"
#include "bnPlayerHitState.h"
#include "bnNaviRegistration.h"

using sf::IntRect;

class Starman : public Player {
public:
  friend class PlayerControlledState;
  friend class PlayerIdleState;

  Starman(void);

};