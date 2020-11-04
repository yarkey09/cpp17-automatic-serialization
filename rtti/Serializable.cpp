//
// Created by zhangyeqi on 2020/11/4.
//

#include "Serializable.h"
#include <memory>

using rtti::Serializable;

std::string Serializable::serial() const {
    return onSerial();
}

void Serializable::unSerial(const std::string& serialData) {
    onUnSerial(serialData);
}

std::string Serializable::onSerial() const {
    return std::string();
}

void Serializable::onUnSerial(const std::string& serialData) {
}