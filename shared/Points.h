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
  Points(df::ViewObjectLocation location = df::TOP_RIGHT);

  // Set value.
  void setValue(int value) override;

  void serialize(std::stringstream &ss) override;
  void deserialize(std::stringstream &ss) override;
};
