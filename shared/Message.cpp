#include "Message.h"

#include <sstream>
#include <string>
#include <cstring>

Message::Message(MessageType type, std::vector<std::byte> &body)
{
    this->type = type;
    this->body = body;
}

Message::Message()
{
    type = MessageType::UNDEFINED;
}

std::vector<std::byte> Message::serialize()
{
    // calculate length including the length itself
    int length = sizeof(int) + sizeof(type) + body.size();
    // instantiate byte buffer with the length
    std::vector<std::byte> buffer(length);
    // copy all data into the buffer, incrementing the pointer each time
    std::memcpy(buffer.data(), &length, sizeof(length));
    std::memcpy(buffer.data() + sizeof(length), &type, sizeof(type));
    std::memcpy(buffer.data() + sizeof(length) + sizeof(type), body.data(), body.size());

    return buffer;
}

void Message::deserialize(std::byte *buffer)
{
    int length;
    // read the length out into an int
    std::memcpy(&length, buffer, sizeof(length));
    // read out the type
    std::memcpy(&type, buffer + sizeof(length), sizeof(type));
    // calculate the size of the contents from the length, resize the field appropriately, and read it out
    body.resize(length - sizeof(length) - sizeof(type));
    std::memcpy(body.data(), buffer + sizeof(length) + sizeof(type), body.size());
}
