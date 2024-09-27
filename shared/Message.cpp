#include "Message.h"

#include <sstream>
#include <string>
#include <cstring>

Message::Message(MessageType type, std::string &body)
{
    this->type = type;
    this->body = body;
}

Message::Message(MessageType type)
{
    this->type = type;
}

Message::Message()
{
    type = MessageType::UNDEFINED;
}

void Message::serialize(std::stringstream &ss)
{
    std::string body_string = body;
    int size = sizeof(int) + sizeof(MessageType) + static_cast<int>(body_string.size());
    ss.write(reinterpret_cast<const char *>(&size), sizeof(size));

    int type_int = static_cast<int>(type);
    ss.write(reinterpret_cast<const char *>(&type_int), sizeof(type_int));

    ss.write(body_string.c_str(), body_string.size());
}

void Message::deserialize(std::stringstream &ss)
{
    int size;
    ss.read(reinterpret_cast<char *>(&size), sizeof(size));

    int type_int;
    ss.read(reinterpret_cast<char *>(&type_int), sizeof(type_int));
    type = static_cast<MessageType>(type_int);

    body.resize(size - sizeof(int) - sizeof(MessageType));
    ss.read(&body[0], body.size());
}
