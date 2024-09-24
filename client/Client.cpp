#include "Client.h"

#include "../shared/Message.h"
#include "ServerEntry.h"
#include "../shared/Sword.h"

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

    LM.writeLog("Message type %d", message.type);

    std::stringstream bs(message.body);

    switch (message.type)
    {
    case MessageType::SYNCHRONIZE:
        int id;
        bs.read(reinterpret_cast<char *>(&id), sizeof(id));
        LM.writeLog("ID: %d", id);
        int object_type_len;
        bs.read(reinterpret_cast<char *>(&object_type_len), sizeof(object_type_len));
        LM.writeLog("object type len: %d", object_type_len);
        std::string type(object_type_len, '\0');
        bs.read(&type[0], object_type_len);
        LM.writeLog("type: %s", type.c_str());
        df::Object *object = WM.objectWithId(id);
        if (object != NULL)
        {
            LM.writeLog("Object found");
            if (dynamic_cast<Serializable *>(object))
            {
                object->deserialize(&bs);
            }
            else
            {
                LM.writeLog("Client::data(): synch message sent with unserializable object");
            }
        }
        else
        {
            LM.writeLog("Creating new object");
            Sword *sword = new Sword();
            sword->deserialize(bs);
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
