#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include "Message.h"

int main()
{
    // Step 1: Create a message
    std::string body = "Hello, World!";
    MessageType type = MessageType::SYNCHRONIZE;
    Message originalMessage(type, body);

    // Step 2: Serialize the message
    std::stringstream ss;
    originalMessage.serialize(ss);

    std::string buffer = ss.str();

    std::stringstream ds(buffer);

    int size;

    ss.read(reinterpret_cast<char *>(&size), sizeof(size));

    std::cout << size << std::endl;

    return 0;
}
