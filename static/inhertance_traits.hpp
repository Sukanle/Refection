#pragma once

#ifndef MAX_TEMPLATE_DEPTH
#define MAX_TEMPLATE_DEPTH std::uint8_t
#endif

#include <cstdint>

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
}   // namespace reflect::Static
