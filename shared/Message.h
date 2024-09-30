#include <cstdint>
#include <string>
#include <vector>

#include "Serializable.h"

// types of messages
enum class MessageType
{
    UNDEFINED = -1,
    SYNCHRONIZE,    // synch an object
    DELETE,         // delete an object
    EXIT,           // a client has exited
    MOUSE_MOVEMENT, // a client has moved their mouse
    GAME_OVER,      // notify clients the game is over
    PING            // get ping
};

// A utility struct for serializing/deserializing messages
struct Message : public Serializable
{
    MessageType type;
    std::string body; // the actual message contents

    Message(MessageType type, std::string &body);
    Message(MessageType type);                        // for messages without a body
    Message();                                        // default construction (used in deserialization)
    void serialize(std::stringstream &ss) override;   // serialize a message to the stream
    void deserialize(std::stringstream &ss) override; // deserialize a message from a stream
};