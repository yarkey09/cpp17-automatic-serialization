//
// Created by zhangyeqi on 2020/10/27.
//

#ifndef CPPRTTI_RTTI_H
#define CPPRTTI_RTTI_H

#include "Serializable.h"
#include "Equals.h"

#include <ostream>
#include <type_traits>

namespace rtti {

    class ClassBase : public Serializable {
    public:
        virtual bool operator==(const ClassBase& rhs) const {
            return false;
        }
    };

    template <typename T>
    struct IsClass {
        static constexpr bool Value =  std::is_base_of_v<ClassBase, T>;
    };

    class FieldBase {
    public:
        virtual ~FieldBase() = default;
        virtual bool operator==(const FieldBase& rhs) const {
            return false;
        }
        virtual bool operator!=(const FieldBase& rhs) const {
            return !(*this == rhs);
        }
    };

    template <typename T>
    struct IsField {
        static constexpr bool Value = std::is_base_of_v<FieldBase, T>;
    };

    constexpr const int FIELD_TYPE_NONE = 0;                // none
    constexpr const int FIELD_TYPE_PRIMARY = 1;             // primary
    constexpr const int FIELD_TYPE_SHARED = 2;              // class
    constexpr const int FIELD_TYPE_PRIMARY_ARRAY = 3;       // primary array
    constexpr const int FIELD_TYPE_SHARED_ARRAY = 4;        // class array

    template<typename UNIT>
    class _ValueField : public FieldBase {
    private:
        UNIT _value;
        bool _has = false;
    public:

        _ValueField() = default;
        _ValueField(const UNIT &value) : _value(value), _has(true) {}

        bool operator==(const FieldBase& rhs) const override {
            const _ValueField<UNIT>* v = dynamic_cast<const _ValueField<UNIT> *>(&rhs);
            if (v == nullptr) {
                return false;
            }
            return _has == v->_has && _value == v->_value;
        }

        _ValueField<UNIT>& operator=(const UNIT& value) {
            this->set(value);
            return *this;
        }

        const UNIT& get() const {
            return _value;
        }
        UNIT& mut_get() {
            return _value;
        }
        void set(const UNIT& value) {
            _value = value;
            _has = true;
        }
        bool has() const {
            return _has;
        }
        void setHas(bool has) {
            _has = has;
        }
    };

    template <typename UNIT>
    class SharedObject {
    private:
        std::shared_ptr<UNIT> _working;
        std::shared_ptr<const UNIT> _stage;
    public:
        SharedObject() : _working(std::make_shared<UNIT>()) {}

        SharedObject(const std::shared_ptr<UNIT>& working) : _working(working) {
        }

        SharedObject(const std::shared_ptr<UNIT>& working, const std::shared_ptr<const UNIT>& stage) : _working(working), _stage(stage) {
        }

        UNIT& operator=(const std::shared_ptr<UNIT>& working) const {
            _working = working;
            return *this;
        }

        UNIT& operator*() const {
            return *_working;
        }

        std::shared_ptr<UNIT>* operator->() const {
            return _working;
        }

        void set(const std::shared_ptr<UNIT>& working) {
            _working = working;
        }

        const std::shared_ptr<UNIT>& get() const {
            return _working;
        }

        void setStage(const std::shared_ptr<const UNIT>& stage) {
            _stage = stage;
        }
        const std::shared_ptr<const UNIT>& getStage() const {
            return _stage;
        }

        bool operator==(const SharedObject<UNIT>& rhs) const {
            return *_working == *rhs._working;
        }
    };

    template <typename, int>
    class FieldTemplate {
    };

    // primary field
    template <typename UNIT>
    class FieldTemplate<UNIT, FIELD_TYPE_PRIMARY> : public _ValueField<UNIT> {
    public:
        static constexpr const int FIELD_TYPE = FIELD_TYPE_PRIMARY;
        using UnitType = UNIT;
        FieldTemplate() = default;
        FieldTemplate(const UNIT &value) : _ValueField<UNIT>(value) {}
    };

    // shared field
    template <typename UNIT>
    class FieldTemplate<UNIT, FIELD_TYPE_SHARED> : public _ValueField<SharedObject<UNIT>> {
    public:
        static constexpr const int FIELD_TYPE = FIELD_TYPE_SHARED;
        using UnitType = UNIT;
        FieldTemplate() = default;
        FieldTemplate(const std::shared_ptr<UNIT>& working) : _ValueField<SharedObject<UNIT>>(SharedObject(working)) {}
    };

    // primary array field
    template <typename UNIT>
    class FieldTemplate<UNIT, FIELD_TYPE_PRIMARY_ARRAY> : public _ValueField<std::vector<UNIT>> {
    public:
        static constexpr const int FIELD_TYPE = FIELD_TYPE_PRIMARY_ARRAY;
        using UnitType = UNIT;
        FieldTemplate() = default;

        // delegate
        std::vector<UNIT>& operator()() {
            this->setHas(true);
            return this->mut_get();
        }
    };

    // class array field
    template <typename UNIT>
    class FieldTemplate<UNIT, FIELD_TYPE_SHARED_ARRAY> : public _ValueField<std::vector<SharedObject<UNIT>>> {
    public:
        static constexpr const int FIELD_TYPE = FIELD_TYPE_SHARED_ARRAY;
        using UnitType = UNIT;
        FieldTemplate() = default;

        // delegate
        std::vector<SharedObject<UNIT>>& operator()() {
            this->setHas(true);
            return this->mut_get();
        }
    };

    template <typename UNIT>
    using Field = FieldTemplate<UNIT, IsClass<UNIT>::Value ? FIELD_TYPE_SHARED : FIELD_TYPE_PRIMARY>;

    template <typename UNIT>
    using ArrayField = FieldTemplate<UNIT, IsClass<UNIT>::Value ? FIELD_TYPE_SHARED_ARRAY : FIELD_TYPE_PRIMARY_ARRAY>;

    template<typename UNIT>
    std::ostream & operator<<(std::ostream &out, const Field<UNIT> &field) {
        out << field.get();
        return out;
    }

    template<typename UNIT>
    std::istream & operator>>(std::istream &in, const Field<UNIT> &field) {
        in >> field.mut_get();
        return in;
    }

    template <typename T, typename F = void, size_t SIZE = 0>
    struct FieldCounter {
        constexpr static size_t Value = SIZE;
    };

    template <typename T, size_t SIZE>
    struct FieldCounter<T, std::enable_if_t<T::template _FieldVisitor<T, SIZE>::VALID>, SIZE> {
        constexpr static size_t Value = FieldCounter<T, void, SIZE+1>::Value;
    };

    /**
     * 遍历所有字段
     */
    template<typename T, typename F, size_t... Is>
    inline constexpr void forEach(T &obj, F &&f, std::index_sequence<Is...>) {
        using TDECAY = std::decay_t<T>;
        (
                f(typename TDECAY::template _FieldVisitor<TDECAY, Is>(obj)),
                ...
        );
    }

    template<typename T, typename F, size_t... Is>
    inline constexpr void forEachConst(const T &obj, F &&f, std::index_sequence<Is...>) {
        using TDECAY = std::decay_t<T>;
        (
                f(
                        typename TDECAY::template _ConstFieldVisitor<TDECAY, Is>(obj)
                ),
                ...
        );
    }

    /**
     * 遍历指定对象的所有成员变量
     *
     * @tparam T 对象类型
     * @tparam F 遍历的回调函数类型
     * @param obj 遍历的对象
     * @param f 遍历的回调函数，例如5个成员变量这个函数会被调用5次
     * @return void
     */
    template<typename T, typename F>
    inline constexpr void forEach(T &obj, F &&f) {
        forEach(obj,
                std::forward<F>(f),
                std::make_index_sequence<rtti::FieldCounter<std::decay_t<T>>::Value>{});
    }

    /**
     * const forEach
     */
    template<typename T, typename F>
    inline constexpr void forEachConst(const T &obj, F &&f) {
        forEachConst(obj,
                std::forward<F>(f),
                std::make_index_sequence<rtti::FieldCounter<std::decay_t<T>>::Value>{});
    }
}

#define RTTI_SUPER_CLASS_START(CLASS_NAME, BASE_CLASS)  struct CLASS_NAME : public BASE_CLASS {                        \
                                                            RTTI_EQUALS(CLASS_NAME)                                    \
                                                            template <typename T, size_t> struct _FieldVisitor {       \
                                                                static constexpr bool VALID = false;                   \
                                                            };                                                         \
                                                            template <typename T, size_t> struct _ConstFieldVisitor {  \
                                                                static constexpr bool VALID = false;                   \
                                                            };

#define RTTI_CLASS_START(CLASS_NAME)                        RTTI_SUPER_CLASS_START(CLASS_NAME, rtti::ClassBase)

#define RTTI_ARRAY_FIELD(INDEX, NAME, UNIT, FEATURE)        _RTTI_FIELD_INTERNAL(INDEX, NAME, UNIT, FEATURE, ArrayField)
#define RTTI_FIELD(INDEX, NAME, UNIT, FEATURE)              _RTTI_FIELD_INTERNAL(INDEX, NAME, UNIT, FEATURE, Field)


#define _RTTI_FIELD_INTERNAL(INDEX, NAME, UNIT, FEATURE, FIELD_CLASS)                                            \
                                                            rtti::FIELD_CLASS<UNIT> NAME;                       \
                                                            template <typename T>                               \
                                                            struct _FieldVisitor<T, INDEX> {                    \
                                                            private:                                            \
                                                                T& obj;                                         \
                                                            public:                                             \
                                                                static constexpr bool VALID = true;             \
                                                                explicit _FieldVisitor(T& obj) : obj(obj) {}    \
                                                                constexpr const char * name() {                 \
                                                                    return #NAME;                               \
                                                                }                                               \
                                                                constexpr const char * feature() const {        \
                                                                    return #FEATURE;                            \
                                                                }                                               \
                                                                rtti::FIELD_CLASS<UNIT>& ref() {                      \
                                                                    return obj.NAME;                            \
                                                                }                                               \
                                                            };                                                  \
                                                                                                                \
                                                            template <typename T>                               \
                                                            struct _ConstFieldVisitor<T, INDEX> {               \
                                                            private:                                            \
                                                                const T& obj;                                   \
                                                            public:                                             \
                                                                static constexpr bool VALID = true;             \
                                                                explicit _ConstFieldVisitor(const T& obj) : obj(obj) {}\
                                                                constexpr const char * name() const {           \
                                                                    return #NAME;                               \
                                                                }                                               \
                                                                constexpr const char * feature() const {        \
                                                                    return #FEATURE;                            \
                                                                }                                               \
                                                                const rtti::FIELD_CLASS<UNIT>& ref() const {    \
                                                                    return obj.NAME;                            \
                                                                }                                               \
                                                            };

#define RTTI_SUPER_CLASS_END(...)                      RTTI_SERIALIZABLE_DEC() };

#define RTTI_CLASS_END(...)                            RTTI_SERIALIZABLE_DEC() };

#endif // CPPRTTI_RTTI_H
