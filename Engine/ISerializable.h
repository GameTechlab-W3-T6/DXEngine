#pragma once
#include "json.hpp"

/**
 * @brief Interface for objects that can be serialized to/from JSON
 */
class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual json::JSON Serialize() const = 0;
    virtual bool Deserialize(const json::JSON& data) = 0;
};