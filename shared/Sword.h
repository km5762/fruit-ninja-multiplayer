//
// Sword.h
//

#ifndef SWORD_H
#define SWORD_H

// Engine includes.
#include "Color.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventStep.h"
#include "Object.h"

#include "Serializable.h"

const char SWORD_CHAR = '+';
const std::string SWORD_STRING = "Sword";

class Sword : public df::Object, public Serializable
{

private:
  df::Color m_color;         // trail color
  df::Vector m_old_position; // previous position
  int m_sliced;              // fruits sliced this move
  int m_old_sliced;          // previous sliced

#ifndef CLIENT
  int m_sock_index;
#endif

  // Handle step event.
  int step(const df::EventStep *p_e);

#ifdef CLIENT
  // Handle mouse event.
  int mouse(const df::EventMouse *p_e);
#endif

public:
  // Constructor.
  Sword(df::Color color = df::Color::CYAN);

  // Handle events.
  int eventHandler(const df::Event *p_e) override;

#ifdef CLIENT
  // Draw.
  int draw(void) override;
#endif

  void serialize(std::stringstream &ss) override;
  // deserealize sword from stream
  void deserialize(std::stringstream &ss) override;

#ifndef CLIENT
  df::Color getColor();
  int getSockIndex();
  void setSockIndex(int sock_index);
#endif
};

#endif // SWORD_H
