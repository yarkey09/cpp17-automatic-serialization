//
// Created by zhangyeqi on 2020/10/30.
//

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Rtti.h"
#include "../json/json.hpp"
#include <iostream>

namespace rtti {

    using SerialStream = nlohmann::json;

    class Serializer {
    public:
        template <typename T>
        static std::unique_ptr<SerialStream> serial(const T& obj) {
            auto stream = std::make_unique<SerialStream>();
            rtti::forEachConst(obj, [&stream](auto&& fieldName, auto&& field) {
                auto fieldValue = field.get();
                (*stream)[fieldName] = fieldValue;
                // std::cout << "serial : " << typeid(decltype(obj)).name() << ", field : " << fieldName << ", value : " << fieldValue << std::endl;
            });
            return stream;
        }

        template <typename T>
        static std::unique_ptr<T> unSerial(const SerialStream& stream) {
            auto obj = std::make_unique<T>();
            rtti::forEach(*obj, [&stream](auto&& fieldName, auto&& field) {
                auto fieldValue = stream[fieldName];
                field.set(fieldValue);
                // std::cout << "unSerial : " << typeid(decltype(*obj)).name() << ", field : " << fieldName << ", value : " << fieldValue << std::endl;
            });
            return obj;
        }
    };
}

#endif //SERIALIZER_H
