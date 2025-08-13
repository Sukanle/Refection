#pragma once

#include "../HLMD/HLMD.h"
#include "config.h"
#include "template_string.hpp"
#include <cstdint>

namespace reflect::Static {

template<typename, typename = void> struct fn_type;

template<typename Ret, typename... Args, typename Class>
struct fn_type<Ret(Args...), Class> {
    using return_t = Ret;
    using class_t = Class;
    using args_t = std::tuple<Args...>;
};
#define SREFL_FNT_HELP(fn, ...)                                         \
    HLMD__VA_OPT__((decltype(HLMD_GET_OTHER_ARGS(__VA_ARGS__)           \
                                 HLMD_GET_FIRST_ARGS(__VA_ARGS__)::fn), \
                    HLMD_GET_FIRST_ARGS(__VA_ARGS__)),                  \
                   (decltype(fn)), HLMD_GET_FIRST_ARGS(__VA_ARGS__))
template<typename Ret, typename... Args, typename Class>
struct fn_type<Ret (*)(Args...), Class> : fn_type<Ret(Args...), Class> {};
template<typename Ret, typename Class, typename... Args>
struct fn_type<Ret (Class::*)(Args...)> {
    using return_t = Ret;
    using class_t = Class;
    using args_t = std::tuple<Args...>;
};

template<typename, typename = void, DEFAULT_TEMPLATE_STRING(, "")>
struct __base_fn_traits;

template<NORMAL_TEMPLATE_STRING(Name), typename Class, typename Ret,
         typename... Args>
struct __base_fn_traits<Ret(Args...), Class, Name>
    : fn_type<Ret(Args...), Class> {
    // static constexpr auto name = NameAccessor<Name>();
    static constexpr auto name = Name;
    static constexpr bool is_member = false;
    static constexpr template_depth params_count = sizeof...(Args);
};
template<NORMAL_TEMPLATE_STRING(Name), typename Class, typename Ret,
         typename... Args>
struct __base_fn_traits<Ret (*)(Args...), Class, Name>
    : __base_fn_traits<Ret(Args...)> {};
template<NORMAL_TEMPLATE_STRING(Name), typename Ret, typename Class,
         typename... Args>
struct __base_fn_traits<Ret (Class::*)(Args...), Class, Name>
    : fn_type<Ret (Class::*)(Args...)> {
    // static constexpr auto name = NameAccessor<Name>();
    static constexpr auto name = Name;
    static constexpr bool is_member = true;
    static constexpr bool is_static = false;
    static constexpr template_depth params_count = sizeof...(Args);
};

template<typename, typename = void, DEFAULT_TEMPLATE_STRING(, "")>
struct fn_traits;

namespace fn_qualify {
inline constexpr uint8_t NOTHING = 0x00;
inline constexpr uint8_t NOEXCEPT = 0x01;
inline constexpr uint8_t CONST = 0x02;
inline constexpr uint8_t VOLATILE = 0x04;
inline constexpr uint8_t CV = 0x06;
inline constexpr uint8_t LVALUE = 0x08;
inline constexpr uint8_t RVALUE = 0x10;
}   // namespace fn_qualify

// NOLINTBEGIN
#define DEF_NOMEM_FN_TRAITS(modifier, value)                             \
    template<NORMAL_TEMPLATE_STRING(Name), typename Class, typename Ret, \
             typename... Args>                                           \
    struct fn_traits<Ret(Args...) modifier, Class, Name>                 \
        : __base_fn_traits<Ret(Args...), Class, Name> {                  \
        using fn_ptr = Ret (*)(Args...) modifier;                        \
        using fn_t = Ret(Args...) modifier;                              \
        using type = fn_t;                                               \
        static constexpr uint8_t modifie = value;                        \
    };                                                                   \
    template<NORMAL_TEMPLATE_STRING(Name), typename Class, typename Ret, \
             typename... Args>                                           \
    struct fn_traits<Ret (*)(Args...) modifier, Class, Name>             \
        : fn_traits<Ret(Args...), Class, Name> {};

DEF_NOMEM_FN_TRAITS(, 0x00)
DEF_NOMEM_FN_TRAITS(noexcept, 0x01)

#define DEF_MEM_FN_TRAITS(modifier, value)                               \
    template<NORMAL_TEMPLATE_STRING(Name), typename Ret, typename Class, \
             typename... Args>                                           \
    struct fn_traits<Ret (Class::*)(Args...) modifier, Class, Name>      \
        : __base_fn_traits<Ret (Class::*)(Args...), Class, Name> {       \
        using m_fn_ptr = Ret (Class::*)(Args...) modifier;               \
        using fn_t = Ret(Class, Args...) modifier;                       \
        using type = fn_t;                                               \
        static constexpr uint8_t modifie = value;                        \
    };
DEF_MEM_FN_TRAITS(, 0x00)
DEF_MEM_FN_TRAITS(const, 0x02)
DEF_MEM_FN_TRAITS(volatile, 0x04)
DEF_MEM_FN_TRAITS(const volatile, 0x06)
DEF_MEM_FN_TRAITS(&, 0x08)
DEF_MEM_FN_TRAITS(const&, 0x0A)
DEF_MEM_FN_TRAITS(volatile&, 0x0C)
DEF_MEM_FN_TRAITS(const volatile&, 0x0E)
DEF_MEM_FN_TRAITS(&&, 0x10)
DEF_MEM_FN_TRAITS(const&&, 0x12)
DEF_MEM_FN_TRAITS(volatile&&, 0x14)
DEF_MEM_FN_TRAITS(const volatile&&, 0x16)
DEF_MEM_FN_TRAITS(noexcept, 0x01)
DEF_MEM_FN_TRAITS(const noexcept, 0x03)
DEF_MEM_FN_TRAITS(volatile noexcept, 0x05)
DEF_MEM_FN_TRAITS(const volatile noexcept, 0x07)
DEF_MEM_FN_TRAITS(& noexcept, 0x09)
DEF_MEM_FN_TRAITS(const& noexcept, 0x0B)
DEF_MEM_FN_TRAITS(volatile& noexcept, 0x0D)
DEF_MEM_FN_TRAITS(const volatile& noexcept, 0x0F)
DEF_MEM_FN_TRAITS(&& noexcept, 0x11)
DEF_MEM_FN_TRAITS(const&& noexcept, 0x13)
DEF_MEM_FN_TRAITS(volatile&& noexcept, 0x15)
DEF_MEM_FN_TRAITS(const volatile&& noexcept, 0x17)
// NOLINTEND

#undef DEF_NOMEM_FN_TRAITS
#undef DEF_MEM_FN_TRAITS
}   // namespace reflect::Static
