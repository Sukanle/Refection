#pragma once

#include "static/config.h"

namespace reflect::Static {

template<typename, typename> struct __base_Inheritance_traits;

// 终止条件：空类型列表
template<typename CurrentDerived>
struct __base_Inheritance_traits<CurrentDerived, empty_list> {
    static constexpr bool value = true;
    using conflict_type = void;
};

// 单继承处理
template<typename CurrentDerived, typename First, typename... Rest>
struct __base_Inheritance_traits<CurrentDerived, type_list<First, Rest...>> {
private:
    using RestList = type_list<Rest...>;

    static constexpr bool is_valid =
        std::is_base_of_v<First, CurrentDerived> ||
        is_virtual_base_of_v<First, CurrentDerived>;

public:
    static constexpr bool value =
        is_valid && __base_Inheritance_traits<First, RestList>::value;

    using conflict_type = std::conditional_t<
        !is_valid, First,
        typename __base_Inheritance_traits<First, RestList>::conflict_type>;
};

template<typename CurrentDerived, typename... Bases, typename other>
struct __base_Inheritance_traits<CurrentDerived,
                                 type_list<type_list<Bases...>, other>> {
private:
    using BaseList = type_list<Bases...>;

public:
    static constexpr bool virtual_check =
        std::disjunction_v<is_virtual_base_of<other, Bases>...>;

    using conflict_type = std::conditional_t<virtual_check, void, other>;
    static constexpr bool value = virtual_check;
};

// 多继承对处理
template<typename CurrentDerived, typename... Bases, typename other,
         typename... Rest>
struct __base_Inheritance_traits<
    CurrentDerived, type_list<type_list<Bases...>, other, Rest...>> {
private:
    using BaseList = type_list<Bases...>;
    static constexpr bool multi_base_check =
        std::disjunction_v<std::is_base_of<Bases, CurrentDerived>...>;
    static constexpr bool virtual_check =
        std::disjunction_v<is_virtual_base_of<other, Bases>...>;

public:
    using conflict_type = std::conditional_t<
        virtual_check,
        typename __base_Inheritance_traits<
            CurrentDerived, type_list<BaseList, Rest...>>::conflict_type,
        other>;

    static constexpr bool value =
        virtual_check &&
        __base_Inheritance_traits<CurrentDerived,
                                  type_list<BaseList, Rest...>>::value;
};

template<typename Derived, typename... Bases> struct Inheritance_traits {
    using Validator = __base_Inheritance_traits<Derived, type_list<Bases...>>;
#ifdef ENABLE_DIAMOND_CHECK
    static_assert(std::is_void_v<typename Validator::conflict_type>,
                  "Diamond inheritance conflict detected. Conflicting base: "
                  "Validator::conflict_type");
#endif

    static constexpr bool value = Validator::value;
    using conflict_base = typename Validator::conflict_type;
};

// TODO: 还有非规则继承链的尚未支持
//
template<typename Derived, typename... Bases>
inline constexpr bool Inheritance_traits_v =
    Inheritance_traits<Derived, Bases...>::value;
template<typename Derived, typename... Bases>
using Inheritance_traits_t =
    typename Inheritance_traits<Derived, Bases...>::conflict_base;
}   // namespace reflect::Static
