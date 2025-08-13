#pragma once
#include <type_traits>

#include "config.h"

namespace reflect::Static::fp {
template<typename T, template_constants N> struct __base_nth;
template<typename T, typename... Remains>
struct __base_nth<type_list<T, Remains...>, 0> {
    using type = T;
};
template<typename T, typename... Remains, template_constants N>
struct __base_nth<type_list<T, Remains...>, N> {
    using type = typename __base_nth<type_list<Remains...>, N - 1>::type;
};
template<typename T> struct __base_head;
template<typename T, typename... Remains>
struct __base_head<type_list<T, Remains...>> {
    using type = T;
};
template<typename T> struct __base_other;
template<typename T, typename... Remains>
struct __base_other<type_list<T, Remains...>> {
    using type = type_list<Remains...>;
};
template<typename TypeList> struct __base_tail {
    static_assert(TypeList::count > 0, "Error: type_list is empty");
    using type = typename __base_nth<TypeList, TypeList::count - 1>::type;
};

template<typename, typename> struct __base_push_front;
template<typename... Args, typename T>
struct __base_push_front<T, type_list<Args...>> {
    using type = type_list<T, Args...>;
    using push_t = T;
};

// 队列式push_back（添加到末尾）
template<typename T, typename TypeList> struct __base_push_back;
template<typename T, typename... Args>
struct __base_push_back<T, type_list<Args...>> {
    using type = type_list<Args..., T>;
    using push_t = T;
};

// 队列式pop_front（移除首元素）
template<typename TypeList> struct __base_pop_front;
template<typename T, typename... Args>
struct __base_pop_front<type_list<T, Args...>> {
    using type = type_list<Args...>;
    using pop_t = T;
};
template<> struct __base_pop_front<type_list<>> {
    using type = type_list<>;
};

// 栈式pop_back（移除末尾元素）
template<typename TypeList> struct __base_pop_back;
template<typename T> struct __base_pop_back<type_list<T>> {
    using type = type_list<>;
    using pop_t = T;
};
template<typename T, typename U, typename... Args>
struct __base_pop_back<type_list<T, U, Args...>> {
    using type = typename __base_push_front<
        T, typename __base_pop_back<type_list<U, Args...>>::type>::type;
    using pop_t = T;
};
template<> struct __base_pop_back<type_list<>> {
    using type = type_list<>;
};

template<typename... Lists> struct __base_concat;
template<typename... Ts> struct __base_concat<type_list<Ts...>> {
    using type = type_list<Ts...>;
};
template<typename... Ts1, typename... Ts2, typename... Rest>
struct __base_concat<type_list<Ts1...>, type_list<Ts2...>, Rest...> {
    using type =
        typename __base_concat<type_list<Ts1..., Ts2...>, Rest...>::type;
};

template<typename> struct __base_size;
template<typename... Args> struct __base_size<type_list<Args...>> {
    static constexpr template_constants value = sizeof...(Args);
};
template<typename, template<typename> class, template_constants>
struct __base_count;
template<typename T, typename... Remains, template<typename> class F>
struct __base_count<type_list<T, Remains...>, F, 0> {
    static constexpr template_constants value = (F<T>::value ? 1 : 0);
};
template<typename T, typename... Remains, template<typename> class F,
         template_constants N>
struct __base_count<type_list<T, Remains...>, F, N> {
    static constexpr template_constants value =
        (F<T>::value ? 1 : 0) +
        __base_count<type_list<Remains...>, F, N - 1>::value;
};

template<typename, template<typename> class, typename> struct __base_map;
template<typename... Remains, template<typename> class F, typename T>
struct __base_map<type_list<Remains...>, F, T> {
    using type =
        type_list<std::conditional_t<F<Remains>::value, T, Remains>...>;
};

template<typename, template<typename...> typename, typename = void>
struct __base_filter;
template<template<typename...> class F, typename AdditionalArg>
struct __base_filter<type_list<>, F, AdditionalArg> {
    using type = type_list<>;
};
template<typename T, typename... Remains, template<typename...> class F>
struct __base_filter<type_list<T, Remains...>, F,
                     std::enable_if_t<F<T>::value>> {
    using type = std::conditional_t<
        F<T>::value,
        typename __base_push_front<
            T, typename __base_filter<type_list<Remains...>, F>::type>::type,
        typename __base_filter<type_list<Remains...>, F>::type>;
};

template<typename T, typename... Remains, template<typename...> class F,
         typename AdditionalArg>
struct __base_filter<type_list<T, Remains...>, F, AdditionalArg> {
    using type = std::conditional_t<
        F<T, AdditionalArg>::value,
        typename __base_push_front<
            T, typename __base_filter<type_list<Remains...>, F,
                                      AdditionalArg>::type>::type,
        typename __base_filter<type_list<Remains...>, F, AdditionalArg>::type>;
};

template<template<typename> class F, typename List> struct __base_transform;
template<template<typename> class F, typename... Ts>
struct __base_transform<F, type_list<Ts...>> {
    using type = type_list<typename F<Ts>::type...>;
};

template<template<typename> class F, typename List> struct __base_flat_map;
template<template<typename> class F, typename... Ts>
struct __base_flat_map<F, type_list<Ts...>> {
    using type = __base_concat<typename F<Ts>::type...>;
};

template<typename List> struct __base_unique;
template<> struct __base_unique<type_list<>> {
    using type = type_list<>;
};
template<typename T, typename... Rest>
struct __base_unique<type_list<T, Rest...>> {
private:
    using rest_unique = typename __base_unique<type_list<Rest...>>::type;

    template<typename U, typename = void> struct contains : std::false_type {};

    template<typename... Us>
    struct contains<type_list<Us...>,
                    std::void_t<decltype((std::is_same_v<T, Us> || ...))>>
        : std::bool_constant<(std::is_same_v<T, Us> || ...)> {};

public:
    using type =
        std::conditional_t<contains<rest_unique>::value, rest_unique,
                           typename __base_push_back<T, rest_unique>::type>;
};

template<typename List, typename Target, template_constants Index = 0>
struct __base_find_index;
template<typename Target, template_constants Index>
struct __base_find_index<type_list<>, Target, Index> {
    static constexpr std::make_signed_t<template_constants> value = -1;
    static_assert(Index < 0, "Error: Target type not found in type_list");
};
template<typename T, typename... Rest, typename Target,
         template_constants Index>
struct __base_find_index<type_list<T, Rest...>, Target, Index> {
    static constexpr std::make_signed_t<template_constants> value =
        std::is_same_v<T, Target>
            ? Index
            : __base_find_index<type_list<Rest...>, Target, Index + 1>::value;
};

template<typename List, typename Target> struct __base_remove;
template<typename Target> struct __base_remove<type_list<>, Target> {
    using type = type_list<>;
};
template<typename T, typename... Rest, typename Target>
struct __base_remove<type_list<T, Rest...>, Target> {
private:
    using rest_removed =
        typename __base_remove<type_list<Rest...>, Target>::type;

public:
    using type =
        std::conditional_t<std::is_same_v<T, Target>, rest_removed,
                           typename __base_push_back<T, rest_removed>::type>;
};

template<typename List, typename Init, template<typename, typename> class Func>
struct __base_fold {
    using type = Init;
};
template<typename Head, typename... Tail, typename Init,
         template<typename, typename> class Func>
struct __base_fold<type_list<Head, Tail...>, Init, Func> {
    using current = typename Func<Init, Head>::type;
    using type = typename __base_fold<type_list<Tail...>, current, Func>::type;
};
}   // namespace reflect::Static::fp
