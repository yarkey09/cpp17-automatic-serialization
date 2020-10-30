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
                using FIELD_VALUE_TYPE = std::decay_t<decltype(fieldValue)>;
                if constexpr (IsRTTIStruct<FIELD_VALUE_TYPE>::Value) {
                    // 下面的代码是正常的，不过反序列化没有相应的实现，所以先不开启；暂不支持类嵌套！
                    throw std::exception();

                    auto fieldStream = serial(fieldValue);
                    (*stream)[fieldName] = *fieldStream;
                } else {
                    // std::cout << "serial : " << typeid(decltype(obj)).name() << ", field : " << fieldName << ", value : " << fieldValue << std::endl;
                    (*stream)[fieldName] = fieldValue;
                }
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
