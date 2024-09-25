#include "Client.h"

#include "../shared/Message.h"
#include "ServerEntry.h"
#include "../shared/Sword.h"
#include "../shared/Fruit.h"
#include "../shared/game.h"
#include "../shared/Timer.h"
#include "../shared/Points.h"

#include "NetworkManager.h"
#include "Event.h"
#include "LogManager.h"
#include "WorldManager.h"

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

                serializable->deserialize(bs);
                df::Object *object = dynamic_cast<Object *>(serializable);
                object->setId(id);
            }
        }
        break;
    case MessageType::DELETE:
        LM.writeLog("recieved req to delete");
        int id;
        if (!bs.read(reinterpret_cast<char *>(&id), sizeof(id)))
        {
            LM.writeLog("error reading id from req to delete");
            break;
        }
        LM.writeLog("recieved req to delete on id %d", id);

        df::Object *object = WM.objectWithId(id);
        if (object != NULL)
        {
            WM.markForDelete(object);
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

    return 0;
}
