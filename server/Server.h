#include "Object.h"
#include "Event.h"
#include "EventNetwork.h"
#include "../shared/Sword.h"

// A df object for handling yak clients
class Server : public df::Object
{
private:
    std::vector<Sword *> swords;
    // handle client messages
    void data(const df::EventNetwork *p_e);
    void accept(const df::EventNetwork *p_e);
    void step(const df::EventStep *p_e);

public:
    Server();
    // handle all events
    int eventHandler(const df::Event *p_e) override;
};
