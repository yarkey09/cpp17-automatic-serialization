//
// Created by zhangyeqi on 2020/11/4.
//

#ifndef CPPRTTI_RTTISERIALIZER_H
#define CPPRTTI_RTTISERIALIZER_H

#include <nlohmann/json_fwd.hpp>

#include <string>
#include <memory>

#define RTTI_KEY_OBJECTS "objects"
#define RTTI_KEY_HEAD "head"

namespace rtti {

    using JsonPtr = nlohmann::json *;
    using ConstJsonPtr = const nlohmann::json *;

    class Serializable {

        friend class Serializer;

    public:
        virtual ~Serializable() = default;

        std::string serial() const;
        void deserial(const std::string& serialData);

    // protected:
        /**
         * serial
         * @param objectsData json object
         * @param key the key of json object
         * @return true if success
         */
        virtual bool onSerial(JsonPtr objectsData, uint64_t key) const;

        /**
         * deserial
         * @param objectsData json object
         * @param key the key of json object
         * @return true if success
         */
        virtual bool onDeserial(ConstJsonPtr objectsData, uint64_t key);
    };
}

#define RTTI_SERIALIZABLE_DEC(...)                                              \
    bool onSerial(rtti::JsonPtr objectsData, uint64_t key) const override;      \
    bool onDeserial(rtti::ConstJsonPtr objectsData, uint64_t key) override;

#define RTTI_SERIALIZABLE_IMP(CLASS_NAME, BASE_CLASS)                           \
bool CLASS_NAME::onSerial(rtti::JsonPtr objectsData, uint64_t key) const {      \
    BASE_CLASS::onSerial(objectsData, key);                                     \
    return rtti::Serializer::serial(*objectsData, *this, key);                  \
}                                                                               \
bool CLASS_NAME::onDeserial(rtti::ConstJsonPtr objectsData, uint64_t key) {     \
    BASE_CLASS::onDeserial(objectsData, key);                                   \
    return rtti::Serializer::unSerial(*objectsData, *this, key);                \
}


#endif //CPPRTTI_RTTISERIALIZER_H
