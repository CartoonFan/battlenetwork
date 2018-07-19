#include "bnAnimate.h"

#include <iostream>

Animate::Animate() {
  this->onFinish = nullptr;
}

Animate::Animate(Animate& rhs) {
  this->onFinish = rhs.onFinish;
  this->callbacks = rhs.callbacks;
}

Animate::~Animate() {
  this->callbacks.clear();
  this->onFinish = nullptr;
}

void Animate::operator() (float progress, sf::Sprite& target, FrameList& sequence)
{
  if (sequence.frames.empty()) {
    if (onFinish != nullptr) {
      onFinish();
    }
    return;
  }

  bool applyCallback = (progress > sequence.totalDuration);

  if (applyCallback) {
    if (onFinish != nullptr) {
      onFinish();
    }
  }

  std::vector<Frame> copy = sequence.frames;

  if ((playbackMode & Mode::Reverse) == Mode::Reverse) {
    reverse(copy.begin(), copy.end());
  }

  int index = 0;
  std::vector<Frame>::const_iterator iter = copy.begin();

  while(iter != copy.end()) {
    index++;
    progress -= (*iter).duration;

    // Must be <= and not <, to handle case (progress == frame.duration) correctly
    if (progress <= 0.f || &(*iter) == &copy.back())
    {
      if ((playbackMode & Mode::Loop) == Mode::Loop && progress > 0.f && &(*iter) == &copy.back()) {
        if ((playbackMode & Mode::Bounce) == Mode::Bounce) {
          reverse(copy.begin(), copy.end());
          iter = copy.begin();
          iter++;
        }
        else {
          iter = copy.begin();
        }

        continue; // Start loop again
      }

      std::map<int, std::function<void()>>::const_iterator callbackIter = this->callbacks.find(index - 1);
      std::map<int, std::function<void()>>::iterator onetimeCallbackIter = this->onetimeCallbacks.find(index - 1);

      if (callbackIter != this->callbacks.end()) {
        if(callbackIter->second)
          callbackIter->second();
      }

      if (onetimeCallbackIter != this->onetimeCallbacks.end()) {
        if(onetimeCallbackIter->second)
          onetimeCallbackIter->second();

        onetimeCallbacks.erase(onetimeCallbackIter);
      }


      target.setTextureRect((*iter).subregion);
      if ((*iter).applyOrigin) {
        target.setOrigin((float)(*iter).origin.x, (float)(*iter).origin.y);
      }

      break;
    }

    iter++;
  }
}

Animate & Animate::operator<<(On& rhs)
{
  if (rhs.doOnce) {
    this->onetimeCallbacks.insert(std::make_pair(rhs.id, rhs.callback));
  }
  else {
    this->callbacks.insert(std::make_pair(rhs.id, rhs.callback));
  }

  return *this;
}

Animate & Animate::operator<<(Mode & rhs)
{
  this->playbackMode = rhs.playback;

  return *this;
}

void Animate::operator<<(std::function<void()> finishNotifier)
{
  this->onFinish = finishNotifier;
}

void Animate::SetFrame(int frameIndex, sf::Sprite & target, FrameList& sequence) const
{
  int index = 0;
  for (Frame& frame : sequence.frames) {
    index++;

    if (index == frameIndex) {
      target.setTextureRect(frame.subregion);
      if (frame.applyOrigin) {
        target.setOrigin((float)frame.origin.x, (float)frame.origin.y);
      }

      return;
    }
  }
}
