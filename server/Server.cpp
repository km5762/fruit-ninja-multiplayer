#include "Server.h"
#include "EventNetwork.h"
#include "NetworkManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "GameManager.h"

#include "../shared/Message.h"
#include "Grocer.h"
#include "../shared/Timer.h"
#include "../shared/Points.h"

#include <vector>

void Server::step(const df::EventStep *p_e)
{
    std::stringstream ss;
    df::ObjectList ol = WM.getAllObjects();
    // for each game object in the world
    for (int i = 0; i < ol.getCount(); i++)
    {
        // if the ID is modified (newly created), or it is a sword, or it is a points and the value has been modified, synch
        if (ol[i]->isModified(df::ObjectAttribute::ID) || (ol[i]->getType() == SWORD_STRING) || (ol[i]->getType() == POINTS_STRING && ol[i]->isModified()))
        {
            int id = ol[i]->getId();
            std::string type = ol[i]->getType();
            // if the object is serializable, serialize it to the string stream
            if (Serializable *serializable = dynamic_cast<Serializable *>(ol[i]))
            {
                ss.write(reinterpret_cast<char *>(&id), sizeof(id));
                // newline delimiter after the type
                ss << type << "\n";
                serializable->serialize(ss);
            }
        }
    }

    std::string body = ss.str();

    // dont sent just the headers if there is no object to synch
    if (!body.empty())
    {
        // wrap the body in a message, and serialize that message to a new string stream
        Message synch_message(MessageType::SYNCHRONIZE, body);

        std::stringstream ms;
        synch_message.serialize(ms);
        std::string message = ms.str();

        // send the synch to all clients
        for (int i = 0; i < NM.getNumConnections(); i++)
        {
            NM.send(message.c_str(), message.size(), i);
        }
    }
}

void Server::data(const df::EventNetwork *p_e)
{
    // read the message into buffer
    int bytes = p_e->getBytes();
    std::vector<char> buffer(bytes);
    NM.receive(buffer.data(), bytes, false, p_e->getSocketIndex());

    std::stringstream ss(std::string(buffer.begin(), buffer.end()));

    // deserialize a message on the buffer
    Message message;
    message.deserialize(ss);

    std::stringstream bs(message.body);
    switch (message.type)
    {
    case MessageType::MOUSE_MOVEMENT:
    {
        // if it is a mose movement, deserialize the position vector from the body and set the corresponding sword's position
        df::Vector position;
        position.deserialize(&bs);
        Sword *sword = swords[p_e->getSocketIndex()];
        sword->setPosition(position);
        break;
    }
    case MessageType::EXIT:
    {
        // get the color of the sword/points of the client who is leaving
        df::Color color = static_cast<df::Color>(p_e->getSocketIndex() + 1);
        df::ObjectList points_displays = WM.objectsOfType(POINTS_STRING);

        // find the point display that corresponds to that client
        for (int i = 0; i < points_displays.getCount(); i++)
        {
            Points *points_display = dynamic_cast<Points *>(points_displays[i]);
            // check if it is the correct display
            if (points_display->getColor() == color)
            {
                WM.markForDelete(points_display);

                // send a message to all clients other than the exiter to delete the points display
                std::stringstream ss;
                int id = points_display->getId();
                ss.write(reinterpret_cast<char *>(&id), sizeof(id));
                std::string body_string = ss.str();
                Message delete_message(MessageType::DELETE, body_string);
                std::stringstream ms;
                delete_message.serialize(ms);
                std::string message = ms.str();
                for (int i = 0; i < NM.getNumConnections(); i++)
                {
                    if (i != p_e->getSocketIndex())
                    {
                        NM.send(message.c_str(), message.length(), i);
                    }
                }
            }
        }

        // shift the sock index of all swords past the client's sword in the sword array, since we are removing a client
        for (int i = p_e->getSocketIndex() + 1; i < swords.size(); i++)
        {
            swords[i]->setSockIndex(swords[i]->getSockIndex() - 1);
        }

        // send a message to all clients other than the exiter to delete the sword
        std::stringstream ss;
        int id = swords[p_e->getSocketIndex()]->getId();
        ss.write(reinterpret_cast<char *>(&id), sizeof(id));
        std::string body_string = ss.str();
        Message delete_message(MessageType::DELETE, body_string);
        std::stringstream ms;
        delete_message.serialize(ms);
        std::string message = ms.str();
        for (int i = 0; i < NM.getNumConnections(); i++)
        {
            if (i != p_e->getSocketIndex())
            {
                LM.writeLog("Sending request to delete sword %d", id);
                NM.send(message.c_str(), message.length(), i);
            }
        }

        // close the connection with that client
        NM.close(p_e->getSocketIndex());
        WM.markForDelete(swords[p_e->getSocketIndex()]);
        // we need to do this to make sure it isnt synched at the end of the step, remove it immediately
        WM.removeObject(swords[p_e->getSocketIndex()]);
        swords.erase(swords.begin() + p_e->getSocketIndex());
        break;
    }
    case MessageType::PING:
    {
        NM.send(buffer.data(), buffer.size(), p_e->getSocketIndex());
        break;
    }
    }
}

void Server::accept(const df::EventNetwork *p_e)
{
    // make a sword with a color corresponding to their socket index
    Sword *sword = new Sword(static_cast<df::Color>(p_e->getSocketIndex() + 1));
    // set the id high to avoid collisions
    sword->setId(100 + p_e->getSocketIndex());
    // set the sock index so we know who to send updates to
    sword->setSockIndex(p_e->getSocketIndex());
    this->swords.push_back(sword);

    // if we have enough people, we can start the game
    if (swords.size() == 2)
    {
        new Grocer();
        new Timer();

        // create a points display for each client with the same color as their sword and a position based on sock index
        for (int i = 0; i < NM.getNumConnections(); i++)
        {
            new Points(static_cast<df::ViewObjectLocation>(i), static_cast<df::Color>(i + 1));
        }
    }
}

Server::Server()
{
    setType("Server");
    setSolidness(df::SPECTRAL);
    setVisible(false);
    registerInterest(df::NETWORK_EVENT);
    registerInterest(df::STEP_EVENT);
    NM.setServer(true);
    NM.setMaxConnections(2); // only 3 people allowed
}

int Server::eventHandler(const df::Event *p_e)
{
    // handle network events
    if (p_e->getType() == df::NETWORK_EVENT)
    {
        df::EventNetwork *event = (df::EventNetwork *)p_e;
        // handle data events
        if (event->getLabel() == df::NetworkEventLabel::DATA)
        {
            data(event);
            return 1;
        }
        // handle accept events
        else if (event->getLabel() == df::NetworkEventLabel::ACCEPT)
        {
            accept(event);
            return 1;
        }
    }
    // handle step events
    else if (p_e->getType() == df::STEP_EVENT)
    {
        df::EventStep *event = (df::EventStep *)p_e;
        step(event);
        return 1;
    }
    return 0;
}
