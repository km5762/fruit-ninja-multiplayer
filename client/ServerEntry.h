#include "TextEntry.h"

// textual interface for entering host information
class ServerEntry : public df::TextEntry
{
public:
    ServerEntry();
    void callback() override;
};
