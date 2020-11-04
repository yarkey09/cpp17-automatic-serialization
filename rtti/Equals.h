//
// Created by zhangyeqi on 2020/11/4.
//

#ifndef CPPRTTI_EQUALS_H
#define CPPRTTI_EQUALS_H

#define RTTI_EQUALS(CLASS_NAME)                                             \
    bool operator==(const ClassBase &rhs) const override {                  \
        const auto *that = dynamic_cast<const CLASS_NAME *>(&rhs);          \
        if (that == nullptr) {                                              \
            return false;                                                   \
        }                                                                   \
        std::vector<const rtti::FieldBase*> fieldsOfThis;                   \
        rtti::forEachConst(*this, [&fieldsOfThis](auto &&fieldVisitor) {    \
            fieldsOfThis.push_back(&fieldVisitor.ref());                    \
        });                                                                 \
        std::vector<const rtti::FieldBase*> fieldsOfThat;                   \
        rtti::forEachConst(*that, [&fieldsOfThat](auto &&fieldVisitor) {    \
            fieldsOfThat.push_back(&fieldVisitor.ref());                    \
        });                                                                 \
                                                                            \
        auto size = fieldsOfThis.size();                                    \
        for (decltype(size) i=0; i<size; i++) {                             \
            if (*fieldsOfThis[i] != *fieldsOfThat[i]) {                     \
                return false;                                               \
            }                                                               \
        }                                                                   \
        return true;                                                        \
    }

#endif //CPPRTTI_EQUALS_H
