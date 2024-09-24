#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"

#include "Server.h"

int main(int argc, char *argv[])
{
    // startup the game manager
    if (GM.startUp())
    {
        LM.writeLog("Error starting game manager!");
        GM.shutDown();
        return 0;
    }

    // setup logging
    LM.setLogLevel(1);
    LM.setFlush(true);
    LM.writeLog("Server (v%.1f)", 1.0);

    // instantiate the server
    new Server();

    GM.run();

    // shut everything down
    GM.shutDown();

    return 0;
}
