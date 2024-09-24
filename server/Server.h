#include "Object.h"
#include "Event.h"
#include "EventNetwork.h"

// A df object for handling yak clients
class Server : public df::Object
{
private:
    // handle client messages
    void handleData(const df::EventNetwork *p_e);

public:
    Server();
    // handle all events
    int eventHandler(const df::Event *p_e) override;
};
