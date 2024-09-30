//
// Points.h
//

// Engine includes.
#include "Event.h"
#include "ViewObject.h"

#include "../shared/Serializable.h"

const std::string POINTS_STRING = "Points";

class Points : public df::ViewObject, public Serializable
{

public:
  // Constructor.
  Points(df::ViewObjectLocation location = df::TOP_RIGHT, df::Color color = df::WHITE);

  // Set value.
  void setValue(int value) override;

  // serialize points to a string stream
  void serialize(std::stringstream &ss) override;
  // deserialize points from a string stream
  void deserialize(std::stringstream &ss) override;
};
