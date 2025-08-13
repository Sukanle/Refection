#pragma once
#include "__base_fp.hpp"

namespace reflect::Static::fp {

template<typename TypeList, template_constants N>
using nth = typename fp::__base_nth<TypeList, N>::type;
template<typename TypeList>
using head = typename fp::__base_head<TypeList>::type;
template<typename TypeList>
using tail = typename fp::__base_tail<TypeList>::type;
template<typename TypeList>
using other = typename fp::__base_other<TypeList>::type;

template<typename TypeList, typename T>
using push_front = __base_push_front<T, TypeList>;   // 头部插入

// 栈、队列式插入
template<typename TypeList, typename T>
using push_back = __base_push_back<T, TypeList>;   // 尾部插入
// 栈式操作别名 (LIFO)
template<typename TypeList>
using pop_back = __base_pop_back<TypeList>;   // 尾部移除
// 队列式操作别名 (FIFO)
template<typename TypeList>
using pop_front = __base_pop_front<TypeList>;   // 头部移除

template<typename... Lists> using concat = typename fp::__base_concat<Lists...>;


template<typename TypeList>
inline constexpr template_constants size = fp::__base_size<TypeList>::value;
template<typename TypeList, template<typename> class F>
inline constexpr template_constants count =
    fp::__base_count<TypeList, F, TypeList::count - 1>::value;

template<typename TypeList, template<typename> class F, typename T>
using map = typename fp::__base_map<TypeList, F, T>::type;
template<template<typename> class F, typename TypeList>
using transform = typename fp::__base_transform<F, TypeList>::type;
template<template<typename> class F, typename List>
using flat_map = typename fp::__base_flat_map<F, List>::type;
template<typename TypeList, template<typename...> class F>
using filter = typename fp::__base_filter<TypeList, F>::type;
template<typename TypeList, template<typename...> class F, typename Arg>
using filter_args = typename fp::__base_filter<TypeList, F, Arg>::type;

template<typename TypeList>
using unique = typename fp::__base_unique<TypeList>::type;
template<typename TypeList, typename Target, template_constants Index = 0>
inline constexpr std::make_signed_t<template_constants> find_index =
    fp::__base_find_index<TypeList, Target, Index>::value;
template<typename TypeList, typename Target>
using remove = typename fp::__base_remove<TypeList, Target>::type;

template<typename List, typename Init, template<typename, typename> class Func>
using fold = typename __base_fold<List, Init, Func>::type;
}   // namespace reflect::Static::fp
