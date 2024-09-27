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
    int bytes = p_e->getBytes();
    std::vector<char> buffer(bytes);
    NM.receive(buffer.data(), bytes, false, p_e->getSocketIndex());

    std::stringstream ss(std::string(buffer.begin(), buffer.end()));

    Message message;
    message.deserialize(ss);

    std::stringstream bs(message.body);

    switch (message.type)
    {
    case MessageType::SYNCHRONIZE:
        while (bs.rdbuf()->in_avail() > 0)
        {
            int id;
            if (!bs.read(reinterpret_cast<char *>(&id), sizeof(id)))
            {
                break;
            }

            std::string type;
            std::getline(bs, type);

            df::Object *object = WM.objectWithId(id);
            if (object != NULL)
            {
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
        int id;
        if (!bs.read(reinterpret_cast<char *>(&id), sizeof(id)))
        {
            break;
        }

        LM.writeLog("request to delete object with ID %d", id);

        df::Object *object_to_delete = WM.objectWithId(id);
        if (object_to_delete != NULL)
        {
            LM.writeLog("request to delete object with ID %d not null", id);
            WM.markForDelete(object_to_delete);
        }
        break;
    }
    case MessageType::GAME_OVER:
        new GameOver();
        break;
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
    else if (p_e->getType() == df::KEYBOARD_EVENT)
    {
        this->keyboard((df::EventKeyboard *)p_e);
        return 1;
    }

    return 0;
}

void Client::keyboard(const df::EventKeyboard *p_e)
{
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
            Message exit_message(MessageType::EXIT);
            std::stringstream ss;
            exit_message.serialize(ss);
            std::string message = ss.str();
            NM.send(message.c_str(), message.length());
            GM.setGameOver(true);
        }
    }
}