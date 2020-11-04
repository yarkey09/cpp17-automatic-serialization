//
// Created by zhangyeqi on 2020/10/30.
//

#ifndef CPPRTTI_SERIALIZER_H
#define CPPRTTI_SERIALIZER_H

// This file/class NOT exported !!

#include "Rtti.h"
#include <nlohmann/json.hpp>
#include <iostream>

namespace rtti {

    using SerialStream = nlohmann::json;

    class Serializer {
    public:

        template <typename T>
        static void serial(SerialStream& objectsStream, const T& obj, uint64_t key) {
            SerialStream stream;
            rtti::forEachConst(obj, [&stream, &objectsStream](auto&& fieldVisitor) {
                decltype(fieldVisitor.name()) fieldName = fieldVisitor.name();
                decltype(fieldVisitor.ref()) fieldRef = fieldVisitor.ref();
                bool fieldHas = fieldRef.has();
                if (!fieldHas) {
                    return;
                }

                const auto& fieldValue = fieldRef.get();
                using FieldType = std::decay_t<decltype(fieldRef)>;

                if constexpr (FieldType::FIELD_TYPE == FIELD_TYPE_SHARED) {
                    // class Field. field value is SharedObject. json is working object key (ptr).
                    auto sharedWorkingObjectRawPtr = fieldValue.get().get();
                    auto sharedObjectKey = reinterpret_cast<uint64_t>(sharedWorkingObjectRawPtr);
                    serial(objectsStream, *sharedWorkingObjectRawPtr, sharedObjectKey);
                    stream[fieldName] = sharedObjectKey;

                } else if constexpr (FieldType::FIELD_TYPE == FIELD_TYPE_PRIMARY_ARRAY) {
                    // primary array Field. field value is std::vector< primary type >
                    SerialStream arrayStream;
                    for (const auto& fieldPrimaryElement : fieldValue) {
                        arrayStream.push_back(fieldPrimaryElement);
                    }
                    stream[fieldName] = arrayStream;

                } else if constexpr (FieldType::FIELD_TYPE == FIELD_TYPE_SHARED_ARRAY) {
                    // class array Field, field value is std::vector< SharedObject >
                    SerialStream arrayStream;
                    for (const auto& fieldSharedElement : fieldValue) {
                        auto sharedWorkingObjectRawPtr = fieldSharedElement.get().get();
                        auto sharedObjectKey = reinterpret_cast<uint64_t>(sharedWorkingObjectRawPtr); // working object ptr
                        arrayStream.push_back(sharedObjectKey);
                        serial(objectsStream, *sharedWorkingObjectRawPtr, sharedObjectKey);
                    }
                    stream[fieldName] = arrayStream;

                } else {
                    // primary field or others. convert to json directly.
                    stream[fieldName] = fieldValue;

                }
            });
            objectsStream[std::to_string(key)] = stream;
        }

        template <typename T>
        static void unSerial(const SerialStream& objectsStream, T& obj, uint64_t key) {
            const SerialStream& stream = objectsStream[std::to_string(key)];
            rtti::forEach(obj, [&stream, &objectsStream](auto&& fieldVisitor) {
                decltype(fieldVisitor.name()) fieldName = fieldVisitor.name();
                decltype(fieldVisitor.ref()) fieldRef = fieldVisitor.ref();
                if (stream.find(fieldName) == stream.end()) {
                    return;
                }
                const auto& streamValue = stream[fieldName];
                using FieldType = std::decay_t<decltype(fieldRef)>;
                using UnitType = typename FieldType::UnitType;

                if constexpr (FieldType::FIELD_TYPE == FIELD_TYPE_SHARED) {
                    // class Field. stream value is working object key, field value is SharedObject.
                    auto fieldObject = std::make_unique<UnitType>();
                    unSerial<UnitType>(objectsStream, *fieldObject, streamValue);
                    fieldRef.set(SharedObject<UnitType>(std::move(fieldObject)));

                } else if constexpr (FieldType::FIELD_TYPE == FIELD_TYPE_PRIMARY_ARRAY) {
                    // primary array field. stream value is array<primary>, field value is std::vector< primary type >
                    std::vector<UnitType> fieldValue;
                    for (const auto& streamPrimaryElement : streamValue) {
                        fieldValue.push_back(streamPrimaryElement);
                    }
                    fieldRef.set(fieldValue);

                } else if constexpr (FieldType::FIELD_TYPE == FIELD_TYPE_SHARED_ARRAY) {
                    // class array field. stream value is array< working object key >, field value is std::vector< SharedObject >
                    std::vector<SharedObject<UnitType>> fieldValue;
                    for (const auto& streamSharedElement : streamValue) {
                        auto fieldElementObject = std::make_unique<UnitType>();
                        unSerial<UnitType>(objectsStream, *fieldElementObject, streamSharedElement);
                        fieldValue.push_back(SharedObject<UnitType>(std::move(fieldElementObject)));
                    }
                    fieldRef.set(fieldValue);

                } else {
                    // primary field or others. convert from json directly.
                    fieldRef.set(streamValue);

                }
            });
        }
    };
}

#endif //CPPRTTI_SERIALIZER_H
