#include "Server.h"
#include "../lib/EventNetwork.h"
#include "../lib/NetworkManager.h"
#include "LogManager.h"

#include "../shared/Message.h"

#include <vector>

void Server::handleData(const df::EventNetwork *p_e)
{
    // read the message into buffer
    int bytes = p_e->getBytes();
    std::vector<std::byte> buffer(bytes);
    NM.receive(buffer.data(), bytes, false, p_e->getSocketIndex());

    // deserialize a message on the buffer
    Message message;
    message.deserialize(buffer.data());

    LM.writeLog("Server::handleData(): recieved message '%s' on socket #%d", message.contents.c_str(), p_e->getSocketIndex());

    // if the message is of type EXIT, handle closing the connection
    if (message.type == MessageType::EXIT)
    {
        LM.writeLog("Server::handleData(): closing connection to socket #%d", p_e->getSocketIndex());
        NM.close(p_e->getSocketIndex());
    }
    // otherwise, send the message to all other connections
    else
    {
        for (int i = 0; i < NM.getNumConnections(); i++)
        {
            if (i != p_e->getSocketIndex())
            {
                NM.send(buffer.data(), bytes, i);
            }
        }
    }
}

Server::Server()
{
    setType("Server");
    setSolidness(df::SPECTRAL);
    setVisible(false);
    registerInterest(df::NETWORK_EVENT);
    // set up NM as server
    NM.setServer(true);
}

int Server::eventHandler(const df::Event *p_e)
{
    // handle network events
    if (p_e->getType() == df::NETWORK_EVENT)
    {
        df::EventNetwork *event = (df::EventNetwork *)p_e;
        // only handle data events
        if (event->getLabel() == df::NetworkEventLabel::DATA)
        {
            this->handleData(event);
            return 1;
        }
    }
    return 0;
}
