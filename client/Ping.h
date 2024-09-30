// Engine includes.
#include "Event.h"
#include "ViewObject.h"
#include "EventStep.h"

const std::string PING_STRING = "Ping";

class Ping : public df::ViewObject
{

private:
    // Handle step events.
    int step(const df::EventStep *p_e);

public:
    // Constructor.
    Ping();

    // Handle events.
    int eventHandler(const df::Event *p_e) override;

    void setValue(int value) override;
};
