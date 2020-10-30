//
// Created by zhangyeqi on 2020/10/27.
//

#ifndef RTTI_H
#define RTTI_H

#include <ostream>

namespace rtti {

    /**
     * 结构体 基类；基类不能加 virtual，否则会应该虚函数表导致无法通过结构体初始化的方式进行初始化，然后 成员计数器也无法工作；
     * 不加 virtual 的话，c++ 的多态性似乎就玩不起来；你无法通过 dynamic_cast 将 StructBase 转型成 子类；
     * 考虑改成 void * 即可
     */
//    struct StructBase {
        // 不要加函数，否则结构体初始化有问题
//        virtual StructBase* clone() const {
//            return new StructBase();
//        }
//    };

    /**
     * 字段 模版基类
     */
//    struct FieldBase {
//    };

    template<typename, bool IS_RTTI_STRUCT>
    struct Field_TEMPLATE;

    /**
     * 基础类型字段，bool, int, float, string ...
     */
    template<typename UNIT>
    struct Field_TEMPLATE<UNIT, false> {
    private:
        UNIT _value;
        bool _has = false;
    public:
        static constexpr bool _RTTI_FIELD = true;
        using ParameterizedType = UNIT;

        Field_TEMPLATE() {} // 必须有默认构造函数，否则 FieldCounter 会计算错误
        Field_TEMPLATE(const UNIT &value) : _value(value), _has(true) {}

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

    /**
     * 非基础类型字段，由 RTTI_STRUCT_START 进行定义的
     */
    template <typename UNIT>
    struct Field_TEMPLATE<UNIT, true> {
    private:
        std::shared_ptr<UNIT> _ptrWorking;
        bool _has = false;

        std::shared_ptr<const UNIT> _ptrStage;

    public:
        static constexpr bool _RTTI_FIELD = true;
        static constexpr bool _RTTI_STRUCT_FIELD = true;
        using ParameterizedType = UNIT;

        Field_TEMPLATE() {}
        Field_TEMPLATE(const std::shared_ptr<UNIT>& working) : _ptrWorking(working), _has(true) {}
        Field_TEMPLATE(const std::shared_ptr<UNIT>& working, const std::shared_ptr<const UNIT>& stage) : _ptrWorking(working), _ptrStage(stage), _has(true) {}

        const std::shared_ptr<UNIT>& get() const {
            return _ptrWorking;
        }
        std::shared_ptr<UNIT>& mut_get() {
            return _ptrWorking;
        }
        void set(const std::shared_ptr<UNIT>& working) {
            _ptrWorking = working;
            _has = true;
        }
        bool has() const {
            return _has;
        }

        // stage
        void setStage(const std::shared_ptr<const UNIT>& stage) {
            _ptrStage = stage;
        }
        std::shared_ptr<const UNIT> getStage() const {
            return _ptrStage;
        }
    };

    /**
     * 判断 T 能否转型为 U
     */
    template <class T, class U>
    class CanConvert {
    private:
        using Small = char;
        using Big = int;

        static Small check (U);
        static Big check (...);

        static T makeT ();

    public:
        static constexpr bool Value = sizeof(check(makeT())) == sizeof(Small);
    };

    /**
     * 判断 T 是否为字段（判断是否包含 NORMAL_FIELD 成员 ）
     * @tparam T
     */
    template <typename T>
    struct IsRTTIField {
        template <typename U>
        static auto check (U) -> typename std::decay<decltype(U::_RTTI_FIELD)>::type;
        static void check(...);
        static constexpr bool Value = !std::is_void_v<decltype(check(std::declval<T>()))>;
    };

    template <typename T>
    struct IsRTTIStructField {
        template <typename U>
        static auto check (U) -> typename std::decay<decltype(U::_RTTI_STRUCT_FIELD)>::type;
        static void check(...);
        static constexpr bool Value = !std::is_void_v<decltype(check(std::declval<T>()))>;
    };

    /**
     * 判断 T 是否为Rtti结构体（判断是否包含 FieldVisitor ）
     * @tparam T
     */
    template <class T>
    class IsRTTIStruct {
    public:
        using Small = char;
        using Big = int;

        template <typename U>
        static Small check (typename U::template FieldVisitor<T, 0> *);

        template <typename U>
        static Big check (...);

        static constexpr bool Value = sizeof(check<T>(0)) == sizeof(Small);
    };

    template <typename UNIT>
    using Field = Field_TEMPLATE<UNIT, IsRTTIStruct<UNIT>::Value>;

    template<typename UNIT>
    std::ostream & operator<<(std::ostream &out, const Field<UNIT> &field) {
        out << field.get();
        return out;
    }

    /**
     * 稻草人，无须实现
     */
    struct Any {
        template<typename T>
        operator T(); // not implemented
    };

    /**
     * 字段计数器，基础模版
     */
    template<typename T, typename = void, typename ...Ts>
    struct FieldCounter {
        constexpr static size_t Value = sizeof...(Ts) - 1; // 本来应该 -1，但是，因为多了基类 StructBase 所以这里 -2
    };

    /**
     * 字段计数器，特化模版
     */
    template<typename T, typename ...Ts>
    struct FieldCounter<T, std::void_t<decltype(T{Ts{}...})>, Ts...> {
        constexpr static size_t Value = FieldCounter<T, void, Ts..., Any>::Value;
    };

    /**
     * 遍历所有字段
     */
    template<typename T, typename F, size_t... Is>
    inline constexpr void forEach(T &obj, F &&f, std::index_sequence<Is...>) {
        using TDECAY = std::decay_t<T>;
        (
                f(typename TDECAY::template FieldVisitor<TDECAY, Is>(obj)),
                ...
        );
    }

    template<typename T, typename F, size_t... Is>
    inline constexpr void forEachConst(const T &obj, F &&f, std::index_sequence<Is...>) {
        using TDECAY = std::decay_t<T>;
        (
                f(
                        typename TDECAY::template ConstFieldVisitor<TDECAY, Is>(obj)
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

#define RTTI_STRUCT_START(STRUCT_NAME)                  struct STRUCT_NAME /* : public rtti::StructBase */ { \
                                                            template <typename T, size_t> struct FieldVisitor; \
                                                            template <typename T, size_t> struct ConstFieldVisitor; \
                                                            static constexpr int _RTTI_STRUCT = 2;

#define RTTI_FIELD(INDEX, NAME, UNIT, FEATURE)              rtti::Field<UNIT> NAME;                             \
                                                                                                                \
                                                            template <typename T>                               \
                                                            struct FieldVisitor<T, INDEX> {                     \
                                                            private:                                            \
                                                                T& obj;                                         \
                                                            public:                                             \
                                                                explicit FieldVisitor(T& obj) : obj(obj) {}     \
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
                                                            struct ConstFieldVisitor<T, INDEX> {                \
                                                            private:                                            \
                                                                const T& obj;                                   \
                                                            public:                                             \
                                                                explicit ConstFieldVisitor(const T& obj) : obj(obj) {}     \
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

#define RTTI_STRUCT_END(...)                            };

#endif // RTTI_H
