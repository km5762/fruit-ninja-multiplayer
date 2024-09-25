//
// Sword.cpp
//

// Engine includes.
#include "DisplayManager.h"
#include "EventCollision.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventView.h"
#include "Fader.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "NetworkManager.h"

// Game includes.
// #include "../shared/Fruit.h"
#include "Sword.h"
#include "Message.h"

#ifdef CLIENT
#include "../client/util.h"
#include "../client/Kudos.h"
#endif

// Constructor.
Sword::Sword(df::Color color)
{

  setType(SWORD_STRING);
  setSolidness(df::SPECTRAL);
  setAltitude(df::MAX_ALTITUDE); // Make Sword in foreground.

  registerInterest(df::STEP_EVENT);
  registerInterest(df::MSE_EVENT);

  // Start sword in center of world.
  df::Vector p(WM.getBoundary().getHorizontal() / 2,
               WM.getBoundary().getVertical() / 2);
  setPosition(p);

  m_old_position = getPosition();
  m_color = color;
  m_sliced = 0;
  m_old_sliced = 0;
}

// Handle event.
// Return 0 if ignored, else 1.
int Sword::eventHandler(const df::Event *p_e)
{
  // Step event.
  if (p_e->getType() == df::STEP_EVENT)
    return step((df::EventStep *)p_e);
#ifdef CLIENT
  else if (p_e->getType() == df::MSE_EVENT)
    return mouse((df::EventMouse *)p_e);
#endif
  // If get here, have ignored this event.
  return 0;
}

// Handle step event.
int Sword::step(const df::EventStep *p_e)
{
  // Check if moved since last step.
  if (m_old_position == getPosition())
  {
    m_sliced = 0;
    return 1;
  }

  // Make a trail from last position to current.

#ifdef CLIENT
  create_trail(getPosition(), m_old_position, m_color);
#endif

  // Check if line intersects any Fruit objects.
  // df::Line line(getPosition(), m_old_position);
  // df::ObjectList ol = WM.solidObjects();

  // for (int i = 0; i < ol.getCount(); i++)
  // {

  //   // Only slice Fruit.
  //   if (!(dynamic_cast<Fruit *>(ol[i])))
  //     continue;

  //   // If line from previous position intersects --> slice!
  //   df::Object *p_o = ol[i];
  //   df::Box box = getWorldBox(p_o);
  //   if (lineIntersectsBox(line, box))
  //   {
  //     df::EventCollision c(this, p_o, p_o->getPosition());
  //     p_o->eventHandler(&c);
  //     m_sliced += 1;

  //     if (!NM.isServer())
  //     {
  //       // Spawn kudos for combo.
  //       if (m_sliced > 2 && m_sliced > m_old_sliced)
  //         new Kudos();
  //     }
  //     m_old_sliced = m_sliced;

  //   } // End of box-line check.

  // } // End of loop through all objects.

  float dist = df::distance(getPosition(), m_old_position);
  // If travel far enough, play "swipe" sound.

#ifdef CLIENT
  if (dist > 15)
  {
    std::string sound = "swipe-" + std::to_string(rand() % 7 + 1);
    play_sound(sound);
  }
#endif

  // Lose points for distance traveled.
  // int penalty = -1 * (int)(dist / 10.0f);
  // df::EventView ev("Points", penalty, true);
  // WM.onEvent(&ev);

  m_old_position = getPosition();

  // Handled.
  return 1;
}

// Draw sword on window.
#ifdef CLIENT
int Sword::draw()
{
  return DM.drawCh(getPosition(), SWORD_CHAR, m_color);
}
#endif

#ifdef CLIENT
int Sword::mouse(const df::EventMouse *p_e)
{
  sf::RenderWindow *p_win = DM.getWindow();
  sf::Vector2i lp = sf::Mouse::getPosition(*p_win);
  if (lp.x > df::Config::getInstance().getWindowHorizontalPixels() ||
      lp.x < 0 ||
      lp.y > df::Config::getInstance().getWindowVerticalPixels() ||
      lp.y < 0)
  {
    return 0;
  }
  else
  {
    // when mosue moves, set position of sword to new position
    if (p_e->getMouseAction() == df::MOVED)
    {
      df::Vector position = p_e->getMousePosition();

      std::stringstream ss;
      position.serialize(&ss);
      std::string body = ss.str();
      Message mouse_message(MessageType::MOUSE_MOVEMENT, body);
      std::stringstream ms;
      mouse_message.serialize(ms);
      std::string message = ms.str();

      int size;
      std::stringstream ts(message);
      ts.read(reinterpret_cast<char *>(&size), sizeof(size));

      NM.send(message.c_str(), message.length());

      return 1;
    }
  }

  return 0;
}
#endif

void Sword::serialize(std::stringstream &ss)
{
  df::Object::serialize(&ss);
  ss.write(reinterpret_cast<char *>(&m_color), sizeof(m_color));
  // this field is a df::Vector, which has its own serialization/desereailization method
  m_old_position.serialize(&ss);
  ss.write(reinterpret_cast<char *>(&m_sliced), sizeof(m_sliced));
  ss.write(reinterpret_cast<char *>(&m_old_sliced), sizeof(m_old_sliced));
}

void Sword::deserialize(std::stringstream &ss)
{
  df::Object::deserialize(&ss);
  ss.read(reinterpret_cast<char *>(&m_color), sizeof(m_color));
  m_old_position.deserialize(&ss);
  ss.read(reinterpret_cast<char *>(&m_sliced), sizeof(m_sliced));
  ss.read(reinterpret_cast<char *>(&m_old_sliced), sizeof(m_old_sliced));
}
