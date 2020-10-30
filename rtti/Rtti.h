//
// Created by zhangyeqi on 2020/10/27.
//

#ifndef RTTI_H
#define RTTI_H

#include "Creator.h"

namespace rtti {

    /**
     * 结构体 基类；基类不能加 virtual，否则会应该虚函数表导致无法通过结构体初始化的方式进行初始化，然后 成员计数器也无法工作；
     * 不加 virtual 的话，c++ 的多态性似乎就玩不起来；你无法通过 dynamic_cast 将 StructBase 转型成 子类
     */
    struct StructBase {
        // 不要加函数，否则结构体初始化有问题
//        virtual StructBase* clone() const {
//            return new StructBase();
//        }
    };

    /**
     * 字段 模版基类
     */
    struct FieldBase {
    };

    /**
     * 字段 模版
     */
    template<typename UNIT>
    struct Field : public FieldBase {
    private:
        UNIT _value;
        bool _has = false;
    public:
        Field() {} // 必须有默认构造函数，否则 FieldCounter 会计算错误
        Field(const UNIT &value) : _value(value), _has(true) {}

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
     * 判断 T 是否为字段（能否转型为 FieldBase）
     * @tparam T
     */
    template <class T>
    class IsField {
    public:
        static constexpr bool Value = CanConvert<T, FieldBase>::Value;
    };

    /**
     * 判断 T 是否为Rtti结构体（能否转型为 StructBase）
     * @tparam T
     */
    template <class T>
    class IsStruct {
    public:
        static constexpr bool Value = CanConvert<T, StructBase>::Value;
    };

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
        constexpr static size_t Value = sizeof...(Ts) - 2; // 本来应该 -1，但是，因为多了基类 StructBase 所以这里 -2
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
                f(
                        typename TDECAY::template FieldVisitor<TDECAY, Is>(obj).name(),
                        typename TDECAY::template FieldVisitor<TDECAY, Is>(obj).ref()
                ),
                ...
        );
    }

    template<typename T, typename F, size_t... Is>
    inline constexpr void forEach(const T &obj, F &&f, std::index_sequence<Is...>) {
        using TDECAY = std::decay_t<T>;
        (
                f(
                        typename TDECAY::template ConstFieldVisitor<TDECAY, Is>(obj).name(),
                        typename TDECAY::template ConstFieldVisitor<TDECAY, Is>(obj).ref()
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
        forEach(std::move(obj),
                std::forward<F>(f),
                std::make_index_sequence<rtti::FieldCounter<std::decay_t<T>>::Value>{});
    }

    /**
     * const forEach
     */
    template<typename T, typename F>
    inline constexpr void forEach(const T &obj, F &&f) {
        forEach(std::move(obj),
                std::forward<F>(f),
                std::make_index_sequence<rtti::FieldCounter<std::decay_t<T>>::Value>{});
    }

    template <typename T>
    std::unique_ptr<T> create(const char * name) {
        return std::make_unique<T>((T*)(Creator::one().create(name)));
    }
}

#define RTTI_STRUCT_START(STRUCT_NAME)                  struct STRUCT_NAME : public rtti::StructBase { \
                                                            template <typename T, size_t> struct FieldVisitor; \
                                                            template <typename T, size_t> struct ConstFieldVisitor; \
                                                            constexpr const char * className() const { return #STRUCT_NAME; } \
                                                            static constexpr const char * staticClassName() { return #STRUCT_NAME; } \
                                                            static STRUCT_NAME* create() { return new STRUCT_NAME(); } \
                                                            STRUCT_NAME* clone() const { return new STRUCT_NAME(*this); } \
                                                            static void registerCreator() { rtti::Creator::one().registerCreateFunc(#STRUCT_NAME, create); }

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
                                                                constexpr const char * name() {                 \
                                                                    return #NAME;                               \
                                                                }                                               \
                                                                const rtti::Field<UNIT>& ref() {                \
                                                                    return obj.NAME;                            \
                                                                }                                               \
                                                            };

#define RTTI_STRUCT_END(...)                            };

#endif // RTTI_H
