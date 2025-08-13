#pragma once

#include <map>
#include <string_view>

namespace reflect::Static {
template<typename E>
struct is_scoped_enum
    : std::integral_constant<
          bool, !std::is_convertible_v<E, typename std::underlying_type_t<E>> &&
                    std::is_enum_v<E>> {};
template<typename E>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;
template<typename E>
struct is_normal_enum
    : std::integral_constant<
          bool, std::is_convertible_v<E, typename std::underlying_type_t<E>> &&
                    std::is_enum_v<E>> {};
template<typename E>
inline constexpr bool is_normal_enum_v = is_normal_enum<E>::value;

template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
struct enum_type {
    using type = E;
    using underlying_t = std::underlying_type_t<E>;
};

template<typename E> struct enum_traits : enum_type<E, void> {
    static std::map<E, std::string_view> list;
    using enum_map_t = typename std::map<E, std::string_view>;
    using enum_pair_t = typename std::map<E, std::string_view>::value_type;
    static constexpr bool is_scoped = is_scoped_enum_v<E>;
    explicit constexpr enum_traits() = default;
    enum_traits(std::initializer_list<enum_pair_t> _list) {
        list = std::map{_list};
    }
};
}   // namespace reflect::Static
