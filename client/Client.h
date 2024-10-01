#include "Object.h"
#include "Event.h"
#include "TextBox.h"
#include "EventNetwork.h"
#include "EventKeyboard.h"

// class for handling connection with fruit ninja server
class Client : public df::Object
{
private:
    df::Color m_color;
    // handle network data events
    void data(const df::EventNetwork *p_e);
    // handle keyboard events
    void keyboard(const df::EventKeyboard *p_e);

    int m_latency;

public:
    Client();
    // handle all events
    int eventHandler(const df::Event *p_e) override;

    df::Color getColor();
};
