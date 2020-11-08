//
// Created by zhangyeqi on 2020/11/5.
//

#include "Serializable.h"
#include <nlohmann/json.hpp>

using rtti::Serializable;

std::string Serializable::serial() const {
    auto key = reinterpret_cast<uint64_t>(this);
    auto rootStream = std::make_unique<nlohmann::json>();
    auto objectsStream = std::make_unique<nlohmann::json>();
    onSerial(objectsStream.get(), key);
    (*rootStream)[RTTI_KEY_OBJECTS] = *objectsStream;
    (*rootStream)[RTTI_KEY_HEAD] = key;
    return rootStream->dump(4);
}

void Serializable::deserial(const std::string &serialData) {
    auto rootStream = nlohmann::json::parse(serialData);
    nlohmann::json &objectsStream = rootStream[RTTI_KEY_OBJECTS];
    uint64_t key = rootStream[RTTI_KEY_HEAD];
    onDeserial(&objectsStream, key);
}

bool Serializable::onSerial(JsonPtr objectsData, uint64_t key) const {
    return false;
}

bool Serializable::onDeserial(ConstJsonPtr objectsData, uint64_t key) {
    return false;
}