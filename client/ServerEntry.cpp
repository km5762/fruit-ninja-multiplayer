#include "Color.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "NetworkManager.h"

#include "ServerEntry.h"
#include "TextBox.h"
#include "../shared/util.h"
#include "Ping.h"

ServerEntry::ServerEntry()
{
    setType("ServerEntry");
    setText("");
    setViewString("enter the host: ");
    setBorder(true);
    setColor(df::WHITE);
    setLocation(df::CENTER_CENTER);
    setLimit(20); // In characters
}

void ServerEntry::callback()
{
    LM.writeLog("ServerEntry::callback(): host: %s", getText().c_str());

    // if the connection fails, we just try again
    if (NM.connect(getText()) == -1)
    {
        setText("");
    }
    // otherwise, delete this and set up the delay
    else
    {
        NM.setDelay(4, 0);
        new Ping();
        WM.markForDelete(this);
    }
}
