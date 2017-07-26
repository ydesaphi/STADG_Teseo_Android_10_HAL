#ifndef TESEO_HAL_UTILS_OPTIONAL_H
#define TESEO_HAL_UTILS_OPTIONAL_H

#if __has_include(<optional>) && __cplusplus > 201402L
#include <optional>
#elif __has_include(<experimental/optional>) && __cplusplus >= 201402L
#include <experimental/optional>
namespace std {

template<typename T>
using optional = std::experimental::optional<T>;

}
#else
#error No optional support in stdlib
#endif

#endif