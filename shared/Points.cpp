//
// Points.cpp
//

// Engine includes
#include "Event.h"
#include "EventStep.h"
#include "LogManager.h"

// Game includes.
#include "Points.h"

Points::Points(df::ViewObjectLocation location, df::Color color)
{
  setType(POINTS_STRING);
  setLocation(location);
  setViewString(POINTS_STRING);
  setColor(color);
  setValue(0);
}

void Points::setValue(int value)
{

  // Call parent.
  ViewObject::setValue(value);

  // If less than 0, set to 0.
  if (getValue() < 0)
    ViewObject::setValue(0);
}

void Points::serialize(std::stringstream &ss)
{
  df::ViewObject::serialize(&ss);
}

void Points::deserialize(std::stringstream &ss)
{
  df::ViewObject::deserialize(&ss);
}
