//
// game.cpp - Fruit Ninja
//

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "utility.h"

// Game includes.
#include "../shared/game.h"
#include "../shared/util.h"
#include "Client.h"

///////////////////////////////////////////////
int main(int argc, char *argv[])
{

  // Start up game manager.
  if (GM.startUp())
  {
    LM.writeLog("Error starting game manager!");
    GM.shutDown();
    return 0;
  }

  // Setup logging.
  LM.setFlush(true);
  LM.setLogLevel(1);
  LM.writeLog("Fruit Ninja (v%.1f)", VERSION);

  // Load resources.
  loadResources();

  // Dragonfly splash screen.
  df::splash();

  // Fruit Ninja splash screen.
  splash();

  // spawn the client
  new Client();

  GM.run();

  // Shut everything down.
  GM.shutDown();

  // All is well.
  return 0;
}
