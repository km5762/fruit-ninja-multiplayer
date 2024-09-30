#include "Client.h"

#include "../shared/Message.h"
#include "ServerEntry.h"
#include "../shared/Sword.h"
#include "../shared/Fruit.h"
#include "../shared/game.h"
#include "../shared/Timer.h"
#include "../shared/Points.h"
#include "../shared/Kudos.h"
#include "GameOver.h"
#include "Ping.h"

#include "NetworkManager.h"
#include "Event.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "GameManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "DisplayManager.h"
#include <SFML/Window/Mouse.hpp>

void Client::data(const df::EventNetwork *p_e)
{
    // read bytes out of socket
    int bytes = p_e->getBytes();
    std::vector<char> buffer(bytes);
    NM.receive(buffer.data(), bytes, false, p_e->getSocketIndex());

    std::stringstream ss(std::string(buffer.begin(), buffer.end()));

    // deserialize a message on the buffer
    Message message;
    message.deserialize(ss);

    // prepare a stringstream on the message's body
    std::stringstream bs(message.body);

    switch (message.type)
    {
    case MessageType::SYNCHRONIZE:
        // in case multiple objects have been sent, we read until the buffer is empty
        while (bs.rdbuf()->in_avail() > 0)
        {
            // read out the id
            int id;
            if (!bs.read(reinterpret_cast<char *>(&id), sizeof(id)))
            {
                break;
            }

            // read out the type (newline delimiter)
            std::string type;
            std::getline(bs, type);

            // try to find the specified object
            df::Object *object = WM.objectWithId(id);
            if (object != NULL)
            {
                // if it exists, deserialize it on the buffer
                if (Serializable *serializable = dynamic_cast<Serializable *>(object))
                {
                    serializable->deserialize(bs);
                }
                else
                {
                    LM.writeLog("Client::data(): synch message sent with unserializable object");
                }
            }
            else
            {
                // otherwise, create a new object based on the type and deserialize it on that
                Serializable *serializable;

                if (type == SWORD_STRING)
                {
                    serializable = new Sword();
                }
                else if (std::find(std::begin(FRUIT), std::end(FRUIT), type) != std::end(FRUIT))
                {
                    serializable = new Fruit(type);
                }
                else if (type == TIMER_STRING)
                {
                    serializable = new Timer();
                }
                else if (type == POINTS_STRING)
                {
                    serializable = new Points();
                }
                else if (type == KUDOS_STRING)
                {
                    serializable = new Kudos();
                }

                serializable->deserialize(bs);
                df::Object *object = dynamic_cast<Object *>(serializable);
                object->setId(id);
            }
        }
        break;
    case MessageType::DELETE:
    {
        // read the id
        int id;
        if (!bs.read(reinterpret_cast<char *>(&id), sizeof(id)))
        {
            break;
        }

        df::Object *object_to_delete = WM.objectWithId(id);
        // if the object with the id exists, delete it
        if (object_to_delete != NULL)
        {
            WM.markForDelete(object_to_delete);
        }
        break;
    }
    case MessageType::GAME_OVER:
        new GameOver();
        break;
    case MessageType::PING:
    {
        int start_time;
        if (!bs.read(reinterpret_cast<char *>(&start_time), sizeof(start_time)))
        {
            break;
        }

        int latency_ticks = GM.getStepCount() - start_time;
        int latency_ms = latency_ticks * GM.getFrameTime();

        df::ObjectList ol = WM.objectsOfType(PING_STRING);

        for (int i = 0; i < ol.getCount(); i++)
        {
            Ping *ping = dynamic_cast<Ping *>(ol[i]);
            ping->setValue(latency_ms);
        }
    }
    }
}

Client::Client()
{
    setType("Client");
    setSolidness(df::SPECTRAL);
    setVisible(false);
    registerInterest(df::NETWORK_EVENT);
    registerInterest(df::STEP_EVENT);
    registerInterest(df::KEYBOARD_EVENT);
    NM.setServer(false);
    // setup serverentry for user to enter the host
    new ServerEntry();
}

int Client::eventHandler(const df::Event *p_e)
{
    // handle network events
    if (p_e->getType() == df::NETWORK_EVENT)
    {
        df::EventNetwork *event = (df::EventNetwork *)p_e;
        // only handle data events
        if (event->getLabel() == df::NetworkEventLabel::DATA)
        {
            this->data(event);
            return 1;
        }
    }
    // handle keyboard events
    else if (p_e->getType() == df::KEYBOARD_EVENT)
    {
        this->keyboard((df::EventKeyboard *)p_e);
        return 1;
    }

    return 0;
}

void Client::keyboard(const df::EventKeyboard *p_e)
{
    // SFML code to make sure we only handle keyboard events when the current window is in focus
    sf::RenderWindow *p_win = DM.getWindow();
    sf::Vector2i lp = sf::Mouse::getPosition(*p_win);
    if (!(lp.x > df::Config::getInstance().getWindowHorizontalPixels() ||
          lp.x < 0 ||
          lp.y > df::Config::getInstance().getWindowVerticalPixels() ||
          lp.y < 0))
    {
        // when q key pressed, end game
        if (p_e->getKey() == df::Keyboard::Q &&
            p_e->getKeyboardAction() == df::KEY_PRESSED)
        {
            // send a message to the server to let it know to close it's connection with this client
            Message exit_message(MessageType::EXIT);
            std::stringstream ss;
            exit_message.serialize(ss);
            std::string message = ss.str();
            NM.send(message.c_str(), message.length());
            GM.setGameOver(true);
        }
    }
}