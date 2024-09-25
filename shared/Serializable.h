#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <sstream>

// interface for serializable objects
class Serializable
{
public:
    // serialize projectile to stream
    virtual void serialize(std::stringstream &p_ss) = 0;

    // deserealize projectile from stream
    virtual void deserialize(std::stringstream &p_ss) = 0;
};

#endif