//
// Created by zhangyeqi on 2020/10/27.
//

#ifndef RTTI_H
#define RTTI_H

#include <ostream>
#include <type_traits>

namespace rtti {

    class _ClassBase {
    public:
        virtual ~_ClassBase() = default;
    };

    template <typename T>
    struct IsClass {
        static constexpr bool Value =  std::is_base_of_v<_ClassBase, T>;
    };

    class _FieldBase {
    public:
        virtual ~_FieldBase() = default;
    };

    template <typename T>
    struct IsField {
        static constexpr bool Value = std::is_base_of_v<_FieldBase, T>;
    };

    template<typename UNIT, bool IS_SHARED_FIELD = false>
    class _ValueField : public _FieldBase {
    private:
        UNIT _value;
        bool _has = false;
    public:
        _ValueField() = default;
        _ValueField(const UNIT &value) : _value(value), _has(true) {}

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
    };

    // primary field
    template <typename UNIT, bool IS_SHARED_FIELD>
    class FieldTemplate : public _ValueField<UNIT, IS_SHARED_FIELD> {
    public:
        static constexpr const bool _SHARED_FIELD = false;
        using UnitType = UNIT;

        FieldTemplate() = default;
        FieldTemplate(const UNIT &value) : _ValueField<UNIT>(value) {}

        auto& operator=(const UNIT& value) {
            this->set(value);
            return *this;
        }
    };

    // shared field
    template <typename UNIT>
    class FieldTemplate<UNIT, true> : public _ValueField<std::shared_ptr<UNIT>> {
    private:
        std::shared_ptr<const UNIT> _ptrStage;

    public:
        static constexpr const bool _SHARED_FIELD = true;
        using UnitType = UNIT;

        FieldTemplate() = default;
        FieldTemplate(const UNIT &value) : _ValueField<std::shared_ptr<UNIT>>(value) {}

        auto& operator=(const std::shared_ptr<UNIT>& value) {
            this->set(value);
            return *this;
        }

        void setStage(const std::shared_ptr<const UNIT>& stage) {
            _ptrStage = stage;
        }
        std::shared_ptr<const UNIT> getStage() const {
            return _ptrStage;
        }
    };

    template <typename UNIT>
    using Field = FieldTemplate<UNIT, IsClass<UNIT>::Value>;

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

    template <typename T>
    struct IsSharedField {
        template <typename U>
        static constexpr bool check (decltype(U::_SHARED_FIELD) *) {
            return U::_SHARED_FIELD;
        }
        template <typename U>
        static constexpr bool check (...) {
            return false;
        }
        static constexpr bool Value = check<T>(0);
    };

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
                                                            template <typename T, size_t> struct _FieldVisitor {       \
                                                                static constexpr bool VALID = false;                   \
                                                            };                                                         \
                                                            template <typename T, size_t> struct _ConstFieldVisitor {  \
                                                                static constexpr bool VALID = false;                   \
                                                            };

#define RTTI_CLASS_START(CLASS_NAME) RTTI_SUPER_CLASS_START(CLASS_NAME, rtti::_ClassBase)

#define RTTI_FIELD(INDEX, NAME, UNIT, FEATURE)              rtti::Field<UNIT> NAME;                             \
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
                                                                rtti::Field<UNIT>& ref() {                      \
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
                                                                const rtti::Field<UNIT>& ref() const {          \
                                                                    return obj.NAME;                            \
                                                                }                                               \
                                                            };

#define RTTI_SUPER_CLASS_END(...)                      };
#define RTTI_CLASS_END(...)                            };

#endif // RTTI_H
