#include <cstdint>
#include <string>
#include <vector>

#include "Serializable.h"

// types of messages
enum class MessageType
{
    UNDEFINED = -1,
    SYNCHRONIZE,
    DELETE,
    EXIT,
    MOUSE_MOVEMENT
};

// A utility struct for serializing/deserializing messages in yak
struct Message : public Serializable
{
    MessageType type;
    std::string body; // the actual message contents

    Message(MessageType type, std::string &body);
    Message(); // default construction (used in deserialization)
    void serialize(std::stringstream &ss) override;
    void deserialize(std::stringstream &ss) override;
};