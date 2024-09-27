#include "Object.h"
#include "Event.h"
#include "TextBox.h"
#include "EventNetwork.h"
#include "EventKeyboard.h"

class Client : public df::Object
{
private:
    void data(const df::EventNetwork *p_e);
    void keyboard(const df::EventKeyboard *p_e);

public:
    Client();
    int eventHandler(const df::Event *p_e) override;
};
