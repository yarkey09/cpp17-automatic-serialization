//
// Created by zhangyeqi on 2020/10/30.
//

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Rtti.h"

// TODO: hide json lib !
#include "../json/json.hpp"

#include <iostream>

namespace rtti {

    using SerialStream = nlohmann::json;

    class Serializer {
    public:
        /**
         * @tparam T obj type
         * @param objectsStream store the obj stream
         * @param obj serial into objectsStream, key is the address of obj.
         * @return the address of obj
         */
        template <typename T>
        static uint64_t serial(SerialStream& objectsStream, const T& obj) {
            SerialStream stream;
            rtti::forEachConst(obj, [&stream, &objectsStream](auto&& fieldVisitor) {
                decltype(fieldVisitor.name()) fieldName = fieldVisitor.name();
                decltype(fieldVisitor.ref()) fieldRef = fieldVisitor.ref();
                auto& fieldValue = fieldRef.get();
                bool fieldHas = fieldRef.has();
                if (!fieldHas) {
                    return;
                }

                // check if it's RTTI Struct Field
                if constexpr (IsSharedField<std::decay_t<decltype(fieldRef)>>::Value) {
                    auto key = serial(objectsStream, *fieldValue); // rtti-struct 's value is shared_ptr
                    stream[fieldName] = key;
                } else {
                    stream[fieldName] = fieldValue;
                }
            });
            auto key = reinterpret_cast<uint64_t>(&obj);
            objectsStream[std::to_string(key)] = stream;
            return key;
        }

        template <typename T>
        static std::unique_ptr<T> unSerial(const SerialStream& objectsStream, uint64_t key) {
            auto obj = std::make_unique<T>();
            const SerialStream& stream = objectsStream[std::to_string(key)];
            rtti::forEach(*obj, [&stream, &objectsStream](auto&& fieldVisitor) {
                decltype(fieldVisitor.name()) fieldName = fieldVisitor.name();
                decltype(fieldVisitor.ref()) fieldRef = fieldVisitor.ref();
                if (stream.find(fieldName) == stream.end()) {
                    return;
                }
                auto& fieldValue = stream[fieldName];

                // check if it's RTTI Struct Field
                using FieldType = std::decay_t<decltype(fieldRef)>;
                if constexpr (IsSharedField<FieldType>::Value) {
                    using FieldUnit = typename FieldType::UnitType;
                    auto fieldObject = unSerial<FieldUnit>(objectsStream, fieldValue); // fieldValue is key
                    fieldRef.set(std::move(fieldObject));
                } else {
                    fieldRef.set(fieldValue);
                }
                // std::cout << "unSerial : " << typeid(decltype(*obj)).name() << ", field : " << fieldName << ", value : " << fieldValue << std::endl;
            });
            return obj;
        }
    };
}

#endif //SERIALIZER_H
