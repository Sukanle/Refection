#pragma once

#include <string_view>   // IWYU pragma: keep
// clang-format off
#ifndef __cpp_consteval
#define consteval constexpr
#endif

#if defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201911L
#include<algorithm>
#define TEMPLATE_STRING_SUPPORTED 1
#else
#define TEMPLATE_STRING_SUPPORTED 0
#endif
// clang-format on
namespace reflect::Static {
// clang-format off
#define DEF2STR(macro_define) #macro_define
#define NUM2STR(num_define) DEF2STR(num_define)

#if TEMPLATE_STRING_SUPPORTED
#define MAX_VARNAME_SIZE_CONIG 32
#define MAX_VARNAME_SIZE (MAX_VARNAME_SIZE_CONIG)

template<uint8_t N> struct template_string {
    static_assert(N <= MAX_VARNAME_SIZE,
                    "Variable name exceeds maximum allowed length "
                    "of " NUM2STR(MAX_VARNAME_SIZE_CONIG));
    consteval template_string(const char (&str)[N]) {
        std::copy(str, str + N, _data);
    }
    [[nodiscard]] consteval const char* data() const noexcept { return _data; }
    [[nodiscard]] consteval uint8_t size() const noexcept { return N - 1; }
    char _data[N];
};
    #define DEFAULT_TEMPLATE_STRING(name, str) \
        reflect::Static::template_string name = reflect::Static::template_string{str}
    #define NORMAL_TEMPLATE_STRING(name) reflect::Static::template_string name
    #define MAKE_TEMPLATE_STRING(str) str
template<template_string Name>
consteval auto NameAccessor(){
    return Name.data();
}
#else
    #define DEFAULT_TEMPLATE_STRING(name, str) const char *name = nullptr
    #define NORMAL_TEMPLATE_STRING(name) const char *name
    #define MAKE_TEMPLATE_STRING(str) nullptr
template<const char *Name>
consteval const char *NameAccessor(){
    return Name;
}
#endif
}   // namespace reflect::Static
// clang-format on
