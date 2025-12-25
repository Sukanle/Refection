#include <cstdint>
#include <type_traits>

#include "config.h"
#include "fp.hpp"

namespace reflect::Static {
template<typename Base, bool IsVirtual> struct base_class_traits {
    using type = Base;
    static constexpr bool is_virtual = IsVirtual;
};
// template<typename T>
// struct class_traits;
template<typename T, typename... Bases> struct class_traits {
    template<typename Base, bool IsVirtual>
    using triats = base_class_traits<Base, IsVirtual>;
};

// --------- 继承检测核心（来自原 type_traits.hpp） ----------
template<typename, typename> struct __base_inheritance_traits;

// 终止条件：空类型列表
template<typename CurrentDerived>
struct __base_inheritance_traits<CurrentDerived, empty_list> {
    static constexpr bool value = true;
    using conflict_type = void;
};

// 单继承处理
template<typename CurrentDerived, typename First, typename... Rest>
struct __base_inheritance_traits<CurrentDerived, type_list<First, Rest...>> {
private:
    using RestList = type_list<Rest...>;

    static constexpr bool is_valid =
        std::is_base_of_v<First, CurrentDerived> ||
        is_virtual_base_of_v<First, CurrentDerived>;

public:
    static constexpr bool value =
        is_valid && __base_inheritance_traits<First, RestList>::value;

    using conflict_type = std::conditional_t<
        !is_valid, First,
        typename __base_inheritance_traits<First, RestList>::conflict_type>;
};

template<typename CurrentDerived, typename... Bases, typename other>
struct __base_inheritance_traits<CurrentDerived,
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
struct __base_inheritance_traits<
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
        typename __base_inheritance_traits<
            CurrentDerived, type_list<BaseList, Rest...>>::conflict_type,
        other>;

    static constexpr bool value =
        virtual_check &&
        __base_inheritance_traits<CurrentDerived,
                                  type_list<BaseList, Rest...>>::value;
};

template<typename Derived, typename... Bases> struct inheritance_traits {
    using Validator = __base_inheritance_traits<Derived, type_list<Bases...>>;
#ifdef ENABLE_DIAMOND_CHECK
    static_assert(std::is_void_v<typename Validator::conflict_type>,
                  "Diamond inheritance conflict detected. Conflicting base: "
                  "Validator::conflict_type");
#endif

    static constexpr bool value = Validator::value;
    using conflict_base = typename Validator::conflict_type;
};

template<typename Derived, typename... Bases>
inline constexpr bool inheritance_traits_v =
    inheritance_traits<Derived, Bases...>::value;
template<typename Derived, typename... Bases>
using inheritance_traits_t =
    typename inheritance_traits<Derived, Bases...>::conflict_base;

// --------- end of inheritance core ----------

// -----------------------------
// 继承链遍历（Inheritance chain traversal）
// 给定一个派生类型和候选类型列表，返回所有可到达的基类集合（按发现顺序），
// 同时避免重复与循环。该功能用于复杂继承链的压力测试。
// 使用示例：
//   using Chain = inheritance_chain_t<Derived, type_list<A,B,C...>>;
//   static_assert(contains_v<Chain, A>);
// -----------------------------

// 元级别：list 包含检测
template<typename List, typename T> struct __contains;
template<typename T> struct __contains<type_list<>, T> : std::false_type {};
template<typename Head, typename... Tail, typename T>
struct __contains<type_list<Head, Tail...>, T>
    : std::bool_constant<std::is_same_v<Head, T> || __contains<type_list<Tail...>, T>::value> {};

template<typename List, typename T>
inline constexpr bool contains_v = __contains<List, T>::value;

// 主收集器：遍历候选列表，收集满足 is_base_of 的类型并递归收集其基类
template<typename Derived, typename Candidates, typename Visited = type_list<>, typename FullCandidates = Candidates>
struct __collect_bases;

// 终止：候选列表为空
template<typename Derived, typename Visited, typename FullCandidates>
struct __collect_bases<Derived, type_list<>, Visited, FullCandidates> {
    using type = type_list<>;
};

// 递归处理候选头部
template<typename Derived, typename Head, typename... Tail, typename Visited, typename FullCandidates>
struct __collect_bases<Derived, type_list<Head, Tail...>, Visited, FullCandidates> {
private:
    using RestCandidates = type_list<Tail...>;
    static constexpr bool is_base = std::is_base_of_v<Head, Derived> && !std::is_same_v<Head, Derived>;
    static constexpr bool already_visited = __contains<Visited, Head>::value;

    // 如果是基类且未访问，则加入并递归收集 Head 的基类
    using NewVisited = std::conditional_t<already_visited, Visited,
                                         typename fp::__base_push_back<Head, Visited>::type>;

    using CollectedFromHead = typename std::conditional_t<
        is_base && !already_visited,
        __collect_bases<Head, FullCandidates, NewVisited, FullCandidates>,
        __collect_bases<Derived, RestCandidates, Visited, FullCandidates>>::type;

    // 继续处理剩余候选项（不论是否加入 Head，都需要继续遍历 Tail）
    using CollectedFromRest = typename __collect_bases<Derived, RestCandidates, Visited, FullCandidates>::type;

    // 如果 Head 被接受，则需要把 Head 放到结果前面并合并 Head 的子结果与剩余结果
    using HeadList = std::conditional_t<is_base && !already_visited, type_list<Head>, type_list<>>;

private:
    using _raw = typename fp::__base_concat<HeadList, CollectedFromHead, CollectedFromRest>::type;
public:
    using type = typename fp::__base_unique<_raw>::type;
};

template<typename Derived, typename Candidates>
struct inheritance_chain {
    using type = typename __collect_bases<Derived, Candidates, type_list<>, Candidates>::type;
    static constexpr std::uint8_t count = type::count;
};

template<typename Derived, typename Candidates>
using inheritance_chain_t = typename inheritance_chain<Derived, Candidates>::type;

}   // namespace reflect::Static

#ifdef REFECTION_INHERTANCE_DEBUG
#include <cstdio>
#include <typeinfo>
#include <string>

namespace reflect::Static::debug {
//TODO: print chain tree to support debug
template<typename...> inline void print_chain_for() {}
template<typename...> inline void print_tree_for(int = 0) {}
} // namespace reflect::Static::debug
#else
namespace reflect::Static::debug {
template<typename...> inline void print_chain_for() {}
template<typename...> inline void print_tree_for(int = 0) {}
} // namespace reflect::Static::debug
#endif

