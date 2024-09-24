#include <cstdint>
#include <string>
#include <vector>

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
struct Message
{
    MessageType type;
    std::vector<std::byte> body; // the actual message contents

    Message(MessageType type, std::vector<std::byte> &contents);
    Message();                           // default construction (used in deserialization)
    std::vector<std::byte> serialize();  // serialize to a vector of bytes
    void deserialize(std::byte *buffer); // deserialize from a pointer to a buffer of bytes
};