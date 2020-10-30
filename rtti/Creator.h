//
// Created by zhangyeqi on 2020/10/29.
//

#ifndef CREATOR_H
#define CREATOR_H

#include <functional>
#include <unordered_map>

namespace rtti {

    class StructBase;

    using CreateFunc = std::function<StructBase*()>;

    class Creator {
    private:
        Creator() = default;
        ~Creator() = default;
        mutable std::unordered_map<const char *, CreateFunc> createFuncs;

    public:
        Creator(const Creator& other) = delete;
        Creator(Creator&& other) = delete;

        static const Creator& one() {
            static Creator one;
            return one;
        }

        void registerCreateFunc(const char * name, CreateFunc func) const {
            createFuncs[name] = std::move(func);
        }

        StructBase* create(const char * name) const {
            auto funcEntry = createFuncs.find(name);
            if (funcEntry == createFuncs.end()) {
                return nullptr;
            }
            return funcEntry->second();
        }
    };

    class CreatorRegister {
    public:
        CreatorRegister(const char * name, CreateFunc func) {
            rtti::Creator::one().registerCreateFunc(name, std::move(func));
        }
    };
}



#endif //CREATOR_H
