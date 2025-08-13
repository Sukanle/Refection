#pragma once
#include "enum_traits.hpp"   // IWYU pragma: keep
#include "fn_traits.hpp"
#include "var_traits.hpp"

namespace reflect::Static {
enum class Kind : uint8_t {
    FreeOrStatic_Var,
    FreeOrStatic_Fn,
    NonStaticMem_Var,
    NonStaticMem_Fn,
};
template<typename T> consteval Kind is_Kind() {
    if constexpr (std::is_member_function_pointer_v<T>)
        return Kind::NonStaticMem_Fn;
    else if constexpr (std::is_member_object_pointer_v<T>)
        return Kind::NonStaticMem_Var;
    else
        return std::is_function_v<T> ? Kind::FreeOrStatic_Fn
                                     : Kind::FreeOrStatic_Var;
}

template<typename T, auto fn_var_t ,typename Class =void, DEFAULT_TEMPLATE_STRING(, "")>
struct __base_field_traits;

template<typename T, typename Class, NORMAL_TEMPLATE_STRING(Name)>
struct __base_field_traits<T, Kind::FreeOrStatic_Var, Class, Name>
    : var_traits<T, Name> {
    using traits = var_traits<T, Name>;
    using type = typename traits::type;

    [[nodiscard]] consteval bool is_member() const { return traits::is_member; }
    [[nodiscard]] consteval bool is_function() const { return false; }
    [[nodiscard]] consteval bool is_variable() const { return true; }
    explicit __base_field_traits(type& ptr)
        : _ptr(ptr) {}
    explicit __base_field_traits(type&& ptr)
        : _ptr(std::move(ptr)) {}

    type _ptr;
};

template<typename T, typename Class, NORMAL_TEMPLATE_STRING(Name)>
struct __base_field_traits<T, Kind::NonStaticMem_Var, Class, Name>
    : var_traits<T, Name> {
    using traits = var_traits<T, Name>;
    using class_t = typename traits::class_t;
    using type = typename traits::type;
    using m_var_ptr = typename traits::m_var_ptr;

    [[nodiscard]] static consteval bool is_member() { return true; }
    [[nodiscard]] static consteval bool is_function() { return false; }
    [[nodiscard]] static consteval bool is_variable() { return true; }
    [[nodiscard]] static consteval bool is_static() { return false; }
    explicit consteval __base_field_traits(m_var_ptr&& ptr)
        : _ptr(std::move(ptr)) {}

    m_var_ptr _ptr;
};

template<typename T, typename Class, NORMAL_TEMPLATE_STRING(Name)>
struct __base_field_traits<T, Kind::FreeOrStatic_Fn, Class, Name>
    : fn_traits<T, Class, Name> {
    using traits = fn_traits<T, Class, Name>;
    using return_t = typename traits::return_t;
    using args_t = typename traits::args_t;
    using function_t = typename traits::function_t;
    using fn_ptr = typename traits::fn_ptr;

    [[nodiscard]] consteval static bool is_member() {
        return traits::is_member;
    }
    [[nodiscard]] consteval static bool is_function() { return true; }
    [[nodiscard]] consteval static bool is_variable() { return false; }
    [[nodiscard]] consteval static template_depth params_count() {
        return traits::params_count;
    }
    explicit __base_field_traits(fn_ptr&& ptr)
        : _ptr(std::move(ptr)) {}

    fn_ptr _ptr;
};
template<typename T, typename Class, NORMAL_TEMPLATE_STRING(Name)>
struct __base_field_traits<T, Kind::NonStaticMem_Fn, Class, Name>
    : fn_traits<T, Class, Name> {
    using traits = fn_traits<T, Class, Name>;
    using class_t = typename traits::class_t;
    using return_t = typename traits::return_t;
    using args_t = typename traits::args_t;
    using function_t = typename traits::function_t;
    using m_fn_ptr = typename traits::m_fn_ptr;

    [[nodiscard]] consteval static bool is_member() {
        return traits::is_member;
    }
    [[nodiscard]] consteval static bool is_static() {
        return traits::is_static;
    }
    [[nodiscard]] consteval static bool is_const() {
        return (traits::modifie & fn_qualify::CONST);
    }
    [[nodiscard]] consteval static bool is_volatile() {
        return (traits::modifie & fn_qualify::VOLATILE);
    }
    [[nodiscard]] consteval static bool is_lvalue() {
        return (traits::modifie & fn_qualify::LVALUE);
    }
    [[nodiscard]] consteval static bool is_rvalue() {
        return (traits::modifie & fn_qualify::RVALUE);
    }
    [[nodiscard]] consteval static bool is_noexcept() {
        return (traits::modifie & fn_qualify::NOEXCEPT);
    }
    [[nodiscard]] consteval static bool is_function() { return true; }
    [[nodiscard]] consteval static bool is_variable() { return false; }
    [[nodiscard]] consteval static template_depth params_count() {
        return traits::params_count;
    }
    explicit consteval __base_field_traits(m_fn_ptr&& ptr)
        : _ptr(std::move(ptr)) {}

    m_fn_ptr _ptr;
};
}   // namespace reflect::Static
