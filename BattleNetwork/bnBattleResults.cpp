#include "bnBattleResults.h"
#include "bnTextureResourceManager.h"
#include "bnEngine.h"
#include "bnMob.h"
#include "bnBattleItem.h"

BattleResults::BattleResults(sf::Time battleLength, int moveCount, int hitCount, int counterCount, bool doubleDelete, bool tripleDelete, Mob *mob) {
  /*
  Calculate score and rank
  Calculations are based off http ://megaman.wikia.com/wiki/Virus_Busting

  Delete Time
    0.00~5.00 = 7
    5.01~12.00 = 6
    12.01~36.00 = 5
    36.01 and up = 4

    Boss Delete Time
    0.00~30.00 = 10
    30.01~40.00 = 8
    40.01~50.00 = 6
    50.01 and up = 4

    Number of Hits(received by MegaMan)
    0 = +1
    1 = 0
    2 = -1
    3 = -2
    4 or more = -3

    Movement Number of Steps
    0~2 = +1
    3 or more = +0

    Multiple Deletion
    Double Delete = +2
    Triple Delete = +4

    Counter Hits
    0 = +0
    1 = +1
    2 = +2
    3 = +3
    */
  score = 0;

  if(!mob->IsBoss()) {
    if (battleLength.asSeconds() > 36.1) score += 4;
    else if (battleLength.asSeconds() > 12.01) score += 5;
    else if (battleLength.asSeconds() > 5.01) score += 6;
    else score += 7;
  }
  else {
    if (battleLength.asSeconds() > 50.01) score += 4;
    else if (battleLength.asSeconds() > 40.01) score += 6;
    else if (battleLength.asSeconds() > 30.01) score += 8;
    else score += 10;
  }

  switch (hitCount) {
    case 0: score += 1; break;
    case 1: score += 0; break;
    case 2: score -= 1; break;
    case 3: score -= 2; break;
    default: score -= 3; break;
  }

  if (moveCount >= 0 && moveCount <= 2) {
    score += 1;
  }

  if (doubleDelete) score += 2;
  if (tripleDelete) score += 4;

  score += std::max(counterCount, 3);

  // No score of zero or below. Min score of 1
  score = std::max(1, score);

  // Get reward based on score
  item = mob->GetRankedReward(score);
 
  isRevealed = false;

  resultsSprite = sf::Sprite(*TEXTURES.GetTexture(TextureType::BATTLE_RESULTS_FRAME));
  resultsSprite.setScale(2.f, 2.f);
  resultsSprite.setPosition(-resultsSprite.getTextureRect().width*2.f, 20.f);

  sf::Font *font = TEXTURES.LoadFontFromFile("resources/fonts/mmbnthick_regular.ttf");

  if (item) {
    sf::IntRect rect = TEXTURES.GetCardRectFromID(item->GetID());

    rewardCard = sf::Sprite(*TEXTURES.GetTexture(TextureType::CHIP_CARDS));
    rewardCard.setTextureRect(rect);

    if (item->IsChip()) {
      chipCode.setFont(*font);
      chipCode.setPosition(2.f*114.f, 209.f);
      chipCode.setString(std::string() + item->GetChipCode());
    }
  }
  else {
    rewardCard = sf::Sprite(*TEXTURES.GetTexture(TextureType::BATTLE_RESULTS_NODATA));
  }

  rewardCard.setScale(2.f, 2.f);
  rewardCard.setPosition(274.0f, 180.f);

  time.setFont(*font);
  time.setPosition(2.f*192.f, 79.f);
  time.setString(FormatString(battleLength));
  time.setOrigin(time.getLocalBounds().width, 0);

  rank.setFont(*font);
  rank.setPosition(2.f*192.f, 111.f);

  reward.setFont(*font);
  reward.setPosition(2.f*42.f, 209.f);

  if (item) {
    reward.setString(item->GetName());
  }
  else {
    reward.setString("No Data");
  }

  if (score > 10) {
    rank.setString("S");
  }
  else {
    rank.setString(std::to_string(score));
  }

  rank.setOrigin(rank.getLocalBounds().width, 0);
}

BattleResults::~BattleResults() {

}

std::string BattleResults::FormatString(sf::Time time)
{
  double totalMS = time.asMilliseconds();
  
  int minutes = (int)totalMS / 1000 / 60;
  int seconds = (int)(totalMS / 1000.0) % 60;
  int ms = (int)(totalMS - (minutes*60*1000) - (seconds*1000)) % 99;

  if (minutes > 59) {
    minutes = 59;
  }

  std::string O = "0";
  std::string builder;

  if (minutes < 10) builder += O;
  builder += std::to_string(minutes) + ":";

  if (seconds < 10) builder += O;
  builder += std::to_string(seconds) + ":";

  if (ms < 10) builder += O;
  builder += std::to_string(ms);

  return builder;
}

// GUI ops
bool BattleResults::CursorAction() {
  bool prevStatus = isRevealed;

  isRevealed = true;

  return prevStatus;
}

bool BattleResults::IsOutOfView() {
  float bounds = -resultsSprite.getTextureRect().width*2.f;

  if (resultsSprite.getPosition().x <= bounds) {
    resultsSprite.setPosition(bounds, resultsSprite.getPosition().y);
  }

  return (resultsSprite.getPosition().x == bounds);
}

bool BattleResults::IsInView() {
  float bounds = 50.f;

  if (resultsSprite.getPosition().x >= bounds) {
    resultsSprite.setPosition(bounds, resultsSprite.getPosition().y);
  }

  return (resultsSprite.getPosition().x == bounds);
}

void BattleResults::Move(sf::Vector2f delta) {
  resultsSprite.setPosition(resultsSprite.getPosition() + delta);
}

void BattleResults::Draw() {
  ENGINE.Draw(resultsSprite, false);

  if (IsInView()) {
    // Draw shadow
    rank.setPosition(2.f*192.f, 112.f);

    if (score > 10) {
      rank.setFillColor(sf::Color(56, 92, 25));
    }
    else {
      rank.setFillColor(sf::Color(80, 72, 88));
    }

    ENGINE.Draw(rank, false);

    // Draw overlay
    rank.setPosition(2.f*191.f, 110.f);

    if (score > 10) {
      rank.setFillColor(sf::Color(176, 228, 24));
    }
    else {
      rank.setFillColor(sf::Color(240, 248, 248));
    }
    ENGINE.Draw(rank, false);

    // Draw shadow
    time.setPosition(2.f*192.f, 80.f);
    time.setFillColor(sf::Color(80, 72, 88));
    ENGINE.Draw(time, false);

    // Draw overlay
    time.setPosition(2.f*191.f, 78.f);
    time.setFillColor(sf::Color(240, 248, 248));
    ENGINE.Draw(time, false);

    if (isRevealed) {
      ENGINE.Draw(rewardCard, false);
      ENGINE.Draw(reward, false);
      
      if (item && item->IsChip()) {
        ENGINE.Draw(chipCode, false);
      }
    }
  }
}

// Chip ops
bool BattleResults::IsFinished() {
  return isRevealed;
}

BattleItem* BattleResults::GetReward()
{
  return item;
}
