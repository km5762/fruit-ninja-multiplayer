//
// Fruit.cpp
//

// Engine includes.
#include "EventCollision.h"
#include "EventOut.h"
#include "EventView.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

// Game includes.
#include "../shared/game.h"
#include "Fruit.h"
// #include "Points.h"
#include "../shared/Sword.h"
#ifdef CLIENT
#include "../shared/util.h"
#endif
#ifndef CLIENT
#include "../shared/Message.h"
#include "NetworkManager.h"
#endif

// Constructor - supply name of Fruit (matches Sprite).
Fruit::Fruit(std::string name)
{

  setType(name);
  if (setSprite(name) != 0)
    LM.writeLog("Fruit::Fruit(): Error! Unable to find sprite: %s",
                name.c_str());
  m_first_out = true; // To ignore first out of bounds (when spawning).
  setSolidness(df::SOFT);
}

// Handle event.
int Fruit::eventHandler(const df::Event *p_e)
{

  // Out of bounds event.
  if (p_e->getType() == df::OUT_EVENT)
    return out((df::EventOut *)p_e);

  // Collision event.
  if (p_e->getType() == df::COLLISION_EVENT)
    return collide((df::EventCollision *)p_e);

  // Not handled.
  return 0;
}

// Handle out events.
int Fruit::out(const df::EventOut *p_e)
{
#ifndef CLIENT
  if (m_first_out)
  { // Ignore first out (when spawning).
    m_first_out = false;
    return 1;
  }

  // Each out is a "miss", so lose points.
  // df::EventView ev(POINTS_STRING, -25, true);
  // WM.onEvent(&ev);

  // Destroy this Fruit.
  WM.markForDelete(this);
#endif

  // Handled.
  return 1;
}

// Handle collision events.
int Fruit::collide(const df::EventCollision *p_e)
{
#ifndef CLIENT
  // Sword collision means ninja sliced this Fruit.
  if (p_e->getObject1()->getType() == SWORD_STRING)
  {

    // Add points.
    // df::EventView ev(POINTS_STRING, +10, true);
    // WM.onEvent(&ev);

    // Destroy this Fruit.
    WM.markForDelete(this);
  }
#endif

  // Handled.
  return 1;
}

// Destructor.
Fruit::~Fruit()
{
#ifdef CLIENT
  // If inside the game world and engine not shutting down,
  // create explosion and play sound.
  if (df::boxContainsPosition(WM.getBoundary(), getPosition()) &&
      GM.getGameOver() == false)
  {
    df::explode(getAnimation().getSprite(), getAnimation().getIndex(), getPosition(),
                EXPLOSION_AGE, EXPLOSION_SPEED, EXPLOSION_ROTATE);

    // Play "splat" sound.
    std::string sound = "splat-" + std::to_string(rand() % 6 + 1);
    play_sound(sound);
  }
#else
  std::stringstream ss;
  int id = getId();
  ss.write(reinterpret_cast<char *>(&id), sizeof(id));
  std::string body_string = ss.str();
  Message delete_message(MessageType::DELETE, body_string);
  std::stringstream ms;
  delete_message.serialize(ms);
  std::string message = ms.str();
  for (int i = 0; i < NM.getNumConnections(); i++)
  {
    NM.send(message.c_str(), message.length(), i);
  }
#endif
}

// Setup starting conditions.
void Fruit::start(float speed)
{

  df::Vector begin, end;

  // Get world boundaries.
  int world_x = (int)WM.getBoundary().getHorizontal();
  int world_y = (int)WM.getBoundary().getVertical();
  df::Vector world_center(world_x / 2.0f, world_y / 2.0f);

  // Pick random side (Top, Right, Bottom, Left) to spawn.
  switch (rand() % 4)
  {

  case 0: // Top.
    begin.setX((float)(rand() % world_x));
    begin.setY(0 - 3.0f);
    end.setX((float)(rand() % world_x));
    end.setY(world_y + 3.0f);
    break;

  case 1: // Right.
    begin.setX(world_x + 3.0f);
    begin.setY((float)(rand() % world_y));
    end.setX(0 - 3.0f);
    end.setY((float)(rand() % world_y));
    break;

  case 2: // Bottom.
    begin.setX((float)(rand() % world_x));
    begin.setY(world_y + 3.0f);
    end.setX((float)(rand() % world_x));
    end.setY(0 - 3.0f);
    break;

  case 3: // Left.
    begin.setX(0 - 3.0f);
    begin.setY((float)(rand() % world_y));
    end.setX(world_x + 3.0f);
    end.setY((float)(rand() % world_y));
    break;

  default:
    break;
  }

  // Move Object into position.
  WM.moveObject(this, begin);

  // Set velocity towards opposite side.
  df::Vector velocity = end - begin;
  velocity.normalize();
  setDirection(velocity);
  setSpeed(speed);
}

void Fruit::serialize(std::stringstream &ss)
{
  df::Object::serialize(&ss);
  ss.write(reinterpret_cast<char *>(&m_first_out), sizeof(m_first_out));
}

void Fruit::deserialize(std::stringstream &ss)
{
  df::Object::deserialize(&ss);
  ss.read(reinterpret_cast<char *>(&m_first_out), sizeof(m_first_out));
}
