//
// Created by zhangyeqi on 2020/11/4.
//

#ifndef CPPRTTI_RTTISERIALIZER_H
#define CPPRTTI_RTTISERIALIZER_H

#include <string>
#include <memory>

#define RTTI_KEY_OBJECTS "objects"
#define RTTI_KEY_HEAD "head"

namespace rtti {

    class Serializable {
    public:
        virtual ~Serializable() = default;

        std::string serial() const;
        void unSerial(const std::string& serialData);

    protected:
        virtual std::string onSerial() const;
        virtual void onUnSerial(const std::string& serialData);
    };
}

#define RTTI_SERIALIZABLE_DEC(...)                                      \
    protected:                                                          \
        std::string onSerial() const override;                          \
        void onUnSerial(const std::string& serialData) override;

#define RTTI_SERIALIZABLE_IMP(CLASS_NAME)                               \
std::string CLASS_NAME::onSerial() const {                              \
    auto key = reinterpret_cast<uint64_t>(this);                        \
    nlohmann::json rootStream;                                          \
    nlohmann::json objectsStream;                                       \
    rtti::Serializer::serial(objectsStream, *this, key);                \
    rootStream[RTTI_KEY_OBJECTS] = objectsStream;                       \
    rootStream[RTTI_KEY_HEAD] = key;                                    \
    return rootStream.dump(4);                                          \
}                                                                       \
void CLASS_NAME::onUnSerial(const std::string& serialData) {            \
    auto rootStream = nlohmann::json::parse(serialData);                \
    nlohmann::json& objectsStream = rootStream[RTTI_KEY_OBJECTS];       \
    uint64_t key = rootStream[RTTI_KEY_HEAD];                           \
    rtti::Serializer::unSerial(objectsStream, *this, key);              \
}

#endif //CPPRTTI_RTTISERIALIZER_H
