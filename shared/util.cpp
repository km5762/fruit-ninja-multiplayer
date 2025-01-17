//
// util.cpp
//

// Engine includes.
#include "Fader.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "utility.h"

// Game includes.
#include "../shared/game.h"
#include "Points.h"
#include "Splash.h"
#include "../shared/Sword.h"
#include "Timer.h"

///////////////////////////////////////////////
// Load resources (sprites, sound effects, music).
void loadResources(void)
{

  // Splash sprites.
  RM.loadSprite("sprites/fruit-splash.txt", "fruit-splash");
  RM.loadSprite("sprites/ninja-splash.txt", "ninja-splash");

  // Fruit sprites.
  for (int i = 0; i < NUM_FRUITS; i++)
  {
    std::string name = "sprites/" + FRUIT[i] + ".txt";
    RM.loadSprite(name, FRUIT[i]);
  }

  // Other sprites.
  RM.loadSprite("sprites/kudos.txt", "kudos");
  RM.loadSprite("sprites/gameover.txt", "gameover");

  // Sounds.
  RM.loadSound("sounds/game-start.wav", "game-start");
  RM.loadSound("sounds/game-over.wav", "game-over");
  RM.loadSound("sounds/impact.wav", "impact");
  RM.loadSound("sounds/beep.wav", "beep");

  for (int i = 1; i <= NUM_SPLATS; i++)
  {
    std::string sound = "splat-" + std::to_string(i);
    std::string file = "sounds/" + sound + ".wav";
    RM.loadSound(file, sound);
  }

  for (int i = 1; i <= NUM_SWIPES; i++)
  {
    std::string sound = "swipe-" + std::to_string(i);
    std::string file = "sounds/" + sound + ".wav";
    RM.loadSound(file, sound);
  }

  for (int i = 1; i <= NUM_KUDOS; i++)
  {
    std::string sound = "kudos-" + std::to_string(i);
    std::string file = "sounds/" + sound + ".wav";
    RM.loadSound(file, sound);
  }
}

///////////////////////////////////////////////
// Fruit Ninja splash screen.
void splash(void)
{
  new Splash();
  GM.run();
}

///////////////////////////////////////////////
// Create trail from p1 to p2.
void create_trail(df::Vector p1, df::Vector p2, df::Color color)
{

  const float size = 2;
  const int age = 20;
  const int opacity = 255;
  unsigned char r, g, b;
  df::colorToRGB(color, r, g, b);

  // Calculate step size for interpolation.
  float dist = df::distance(p1, p2) * 10;
  float dX = (p1.getX() - p2.getX()) / (dist + 1.0f);
  float dY = (p1.getY() - p2.getY()) / (dist + 1.0f);

  // Create Fader particles on line from p1 to p2.
  for (int i = 0; i < dist; i++)
  {
    float x = p2.getX() + dX * i;
    float y = p2.getY() + dY * i;
    df::Fader *p_f = new df::Fader(size, age, opacity, r, g, b);
    p_f->setPosition(df::Vector(x, y));
  }
}

// Play indicated sound.
void play_sound(std::string sound)
{
  df::Sound *p_sound = RM.getSound(sound);
  if (p_sound)
    p_sound->play();
  else
    LM.writeLog("play_sound(): Unable to get sound '%s'", sound.c_str());
}
