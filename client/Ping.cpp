#include "Ping.h"

#include "NetworkManager.h"
#include "GameManager.h"
#include "LogManager.h"

#include "../shared/Message.h"

Ping::Ping()
{
    setType(PING_STRING);
    setLocation(df::BOTTOM_LEFT);
    setViewString(PING_STRING);
    setColor(df::WHITE);
    registerInterest(df::STEP_EVENT);
    setValue(50);
}

int Ping::eventHandler(const df::Event *p_e)
{
    // Step events.
    if (p_e->getType() == df::STEP_EVENT)
        return step((df::EventStep *)p_e);

    // If get here, have ignored this event.
    return 0;
}

int Ping::step(const df::EventStep *p_e)
{
    // every 15 steps, update the ping
    if (p_e->getStepCount() % 15 == 0)
    {
        int start_time = GM.getStepCount();

        std::stringstream ss;
        ss.write(reinterpret_cast<char *>(&start_time), sizeof(start_time));
        std::string body = ss.str();

        Message ping(MessageType::PING, body);

        std::stringstream ms;
        ping.serialize(ms);

        std::string message = ms.str();

        // send a ping message to the server with the start time of the ping message
        NM.send(message.c_str(), message.length());
    }

    return 0;
}

void Ping::setValue(int value)
{
    ViewObject::setValue(value);

    // set color of ping based on its value
    if (value < 100)
    {
        setColor(df::GREEN);
    }
    else if (value < 300)
    {
        setColor(df::YELLOW);
    }
    else
    {
        setColor(df::RED);
    }
}
