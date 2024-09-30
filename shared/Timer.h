//
// Timer.h
//

#ifndef TIMER_H
#define TIMER_H

// Engine includes.
#include "Event.h"
#include "ViewObject.h"
#include "Serializable.h"

const std::string TIMER_STRING = "Time";

class Timer : public df::ViewObject, public Serializable
{

private:
  // Handle step events.
  int step(const df::EventStep *p_e);

public:
  // Constructor.
  Timer();

  // Handle events.
  int eventHandler(const df::Event *p_e) override;

  // serialize timer to string stream
  void serialize(std::stringstream &ss) override;
  // desserialize timer from string stream
  void deserialize(std::stringstream &ss) override;
};

#endif // TIMER_H
