#include "Server.h"
#include "EventNetwork.h"
#include "NetworkManager.h"
#include "LogManager.h"
#include "WorldManager.h"

#include "../shared/Message.h"
#include "Grocer.h"
#include "../shared/Timer.h"
#include "../shared/Points.h"

#include <vector>

void Server::step(const df::EventStep *p_e)
{
    std::stringstream ss;
    df::ObjectList ol = WM.getAllObjects();
    for (int i = 0; i < ol.getCount(); i++)
    {
        if (ol[i]->isModified(df::ObjectAttribute::ID) || (ol[i]->getType() == SWORD_STRING) || (ol[i]->getType() == POINTS_STRING && ol[i]->isModified()))
        {
            int id = ol[i]->getId();
            std::string type = ol[i]->getType();
            // if the object is serializable, serialize it to the string stream
            if (Serializable *serializable = dynamic_cast<Serializable *>(ol[i]))
            {
                ss.write(reinterpret_cast<char *>(&id), sizeof(id));
                ss << type << "\n";
                serializable->serialize(ss);
            }
        }
    }

    std::string body = ss.str();

    if (!body.empty())
    {
        Message synch_message(MessageType::SYNCHRONIZE, body);

        std::stringstream ms;
        synch_message.serialize(ms);
        std::string message = ms.str();

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
        df::Vector position;
        position.deserialize(&bs);
        Sword *sword = swords[p_e->getSocketIndex()];
        sword->setPosition(position);
    }
}

void Server::accept(const df::EventNetwork *p_e)
{
    Sword *sword = new Sword(static_cast<df::Color>(p_e->getSocketIndex() + 1));
    sword->setId(100 + p_e->getSocketIndex());
    this->swords.push_back(sword);

    if (swords.size() == 2)
    {
        new Grocer();
        new Timer();

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
}

int Server::eventHandler(const df::Event *p_e)
{
    if (p_e->getType() == df::NETWORK_EVENT)
    {
        df::EventNetwork *event = (df::EventNetwork *)p_e;
        if (event->getLabel() == df::NetworkEventLabel::DATA)
        {
            data(event);
            return 1;
        }
        else if (event->getLabel() == df::NetworkEventLabel::ACCEPT)
        {
            accept(event);
            return 1;
        }
    }
    else if (p_e->getType() == df::STEP_EVENT)
    {
        df::EventStep *event = (df::EventStep *)p_e;
        step(event);
        return 1;
    }
    return 0;
}
