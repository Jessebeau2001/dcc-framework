#pragma once

namespace traits {

// Is Same
template <typename T, typename U>
struct is_same {
    static constexpr bool value = false;
};

template <typename T>
struct is_same<T, T> {
    static constexpr bool value = true;
};

template <typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;


// Enable If
template <bool B, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> { using type = T; };

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;


// Remove CV
template <typename T> struct remove_cv                   { using type = T; };
template <typename T> struct remove_cv<const T>          { using type = T; };
template <typename T> struct remove_cv<volatile T>       { using type = T; };
template <typename T> struct remove_cv<const volatile T> { using type = T; };

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

} // namespace cpt