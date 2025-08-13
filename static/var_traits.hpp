#pragma once

#include "template_string.hpp"

namespace reflect::Static {

template<typename T> struct var_type {
    using type = T;
    using var_ptr = T*;
};
template<typename Class, typename T> struct var_type<T Class::*> {
    using m_var_ptr = T Class::*;
    using class_t = Class;
    using type = T;
};

template<typename T, DEFAULT_TEMPLATE_STRING(Name, "")>
struct __base_var_traits : var_type<T> {
    static constexpr auto name = Name;
    static constexpr bool is_const =
        std::is_const_v<std::remove_reference_t<T>>;
    static constexpr bool is_volatile =
        std::is_volatile_v<std::remove_reference_t<T>>;
};

template<typename T, DEFAULT_TEMPLATE_STRING(Name, "")>
struct var_traits : __base_var_traits<T, Name> {
    static constexpr bool is_member = false;
};

template<typename T, typename Class, NORMAL_TEMPLATE_STRING(Name)>
struct var_traits<T Class::*, Name> : __base_var_traits<T Class::*, Name> {
    static constexpr bool is_member = false;
};

}   // namespace reflect::Static
