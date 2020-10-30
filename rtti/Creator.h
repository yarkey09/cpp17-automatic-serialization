//
// Created by zhangyeqi on 2020/10/29.
//

#ifndef CREATOR_H
#define CREATOR_H

#include "Rtti.h"

#include <functional>
#include <unordered_map>

namespace rtti {

    using Createe = StructBase;
    using CreateFunc = std::function<Createe()>;

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

        Createe create(const char * name) const {
            auto funcEntry = createFuncs.find(name);
            if (funcEntry == createFuncs.end()) {
                return nullptr;
            }
            return funcEntry->second();
        }

        template <typename T>
        std::unique_ptr<T> create(const char * name) const {
            return std::unique_ptr<T>((T*)(create(name)));
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
