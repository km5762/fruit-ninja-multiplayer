#include "Object.h"
#include "Event.h"
#include "EventNetwork.h"
#include "../shared/Sword.h"

// class for handling client connections/server game state
class Server : public df::Object
{
private:
    // vector to hold clients swords
    std::vector<Sword *> swords;
    // handle client messages
    void data(const df::EventNetwork *p_e);
    // handle accepting a new client
    void accept(const df::EventNetwork *p_e);
    // handle steps
    void step(const df::EventStep *p_e);

public:
    Server();
    // handle all events
    int eventHandler(const df::Event *p_e) override;
};
