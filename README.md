# Roka

## Overview

Roka is a very small header-only implementation of addition and multiplication of multi-word unsigned integers in C++17.
It is tested on gcc 7.3, Clang 7.0 and Visual Studio 2019.
It is available under [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/deed).

In Roka, multi-word integeral values are represented as an instance of `std::array<T, N>` where `T` is an unsigned integer type and `N` is the count of the words.
So, inherently, Roka can only handle statically sized integral values.

Words in an instance of `std::array<T, N>` shall be arranged in the big endian manner.
(This design was resulted from the need of comparing two products of some integral values in a possible application of Roka; `std::array<T, N>` provides lexicographical comparison facilities by `operator<` and its comrades.)

Roka's implementation does not employ dynamic memory allocation at all.
Nor does it throw any exceptions.

All functions of Roka are `constexpr`, so you can do compile-time calculation with them.

## Example

```C++
#include <array>
#include <cstdint>

#include <roka/roka.hpp>

void f() {
  constexpr std::uint32_t m = 0xffffffff;
  constexpr std::uint32_t n = 0xffff0001;

  constexpr std::array<std::uint32_t, 2> s = roka::add(m, n);
  static_assert(s[0] == 0x00000001);
  static_assert(s[1] == 0xffff0000);

  constexpr std::array<std::uint32_t, 3> p = roka::mul(s, n);
  static_assert(p[0] == 0x00000001);
  static_assert(p[1] == 0xfffd0002);
  static_assert(p[2] == 0xffff0000);
}
```

## Reference

This clause gives the specification of Roka.

### Normativity
 - The header of Roka can have additional `#include`s other than described here.
 - Function templates shall not have any additional template parameters no matter whether they have their default template arguments.
 - An occurrence of `see_below` in place of a type name does not mean a type name actually;
   it is a placeholder of a type whose detail is going to be specified later.

### For users of Roka
 - A translation unit may include the header of Roka more than once, with no effect different from being included exactly once.
 - A translation unit shall include the header of Roka only outside of any external declaration or definition, and shall include the header lexically before the first reference in that translation unit to any of the entities declared in that header.
 - A translation unit shall not define a macro whose name meets one of the following before a point of an `#include` preprocessor directive that includes the header of Roka:
   - it consists only of one uppercase alphabetic letter,
   - it begins with a lowercase alphabetic letter,
   - it consists only of alphabetic letters and possibly zero decimal digit characters, contains both of an uppercase alphabetic letter and a lowercase one, and begins with an uppercase one, or
   - it begins with `ROKA_`.
 - The header of Roka may define macros whose name begin with `ROKA_` and may not define any macros with other names.

### Header

Roka consists only of one header: `"roka/roka.hpp"`.
Its content is as follows:

```C++
#include <array>
#include <cstddef>

namespace roka {

  // Addition
  template <class UInt>
    constexpr std::array<UInt, 2> add(UInt i, UInt j) noexcept;
  template <class UInt, std::size_t N>
    constexpr see_below add(const std::array<UInt, N>& i, UInt j) noexcept;
  template <class UInt, std::size_t N>
    constexpr see_below add(UInt i, const std::array<UInt, N>& j) noexcept;
  template <class UInt, std::size_t M, std::size_t N>
    constexpr see_below add(
      const std::array<UInt, M>& i, const std::array<UInt, N>& j) noexcept;

  // Multiplication
  template <class UInt>
    constexpr std::array<UInt, 2> mul(UInt i, UInt j) noexcept;
  template <class UInt, std::size_t N>
    constexpr see_below mul(const std::array<UInt, N>& i, UInt j) noexcept;
  template <class UInt, std::size_t N>
    constexpr see_below mul(UInt i, const std::array<UInt, N>& j) noexcept;
  template <class UInt, std::size_t M, std::size_t N>
    constexpr see_below mul(
      const std::array<UInt, M>& i, const std::array<UInt, N>& j) noexcept;

}

```

### Function templates

All function templates of Roka resides in namespace `roka`. They treat an instance of `std::array<T, 0>` as if it represents 0.

#### Addition

```C++
template <class UInt>
  constexpr std::array<UInt, 2> add(UInt i, UInt j) noexcept;               // (1)
template <class UInt, std::size_t N>
  constexpr see_below add(const std::array<UInt, N>& i, UInt j) noexcept;   // (2)
template <class UInt, std::size_t N>
  constexpr see_below add(UInt i, const std::array<UInt, N>& j) noexcept;   // (3)
template <class UInt, std::size_t M, std::size_t N>
  constexpr see_below add(
    const std::array<UInt, M>& i, const std::array<UInt, N>& j) noexcept;   // (4)
```

In this subcaluse, `MAX(a, b)` denotes the greater of `a` and `b`.

_Requires:_ `UInt` shall be an unsigned integer type.

_Returns:_ The sum of `i` and `j` possibly represented as a big endian sequence of `UInt` words. The return type of (2) and (3) is `std::array<UInt, N + 1>` if `N` is not `0`, `std::array<UInt, N>` otherwise. The return type of (4) is `std::array<UInt, MAX(M, N) + 1>` if neither `M` nor `N` is `0`, `std::array<UInt, MAX(M, N)>` if only one of `M` and `N` is `0`, `UInt` otherwise.

_Remark:_ (1) shall not participate in overload resolution unless `UInt` is an integer type.

#### Multiplication

```C++
template <class UInt>
  constexpr std::array<UInt, 2> mul(UInt i, UInt j) noexcept;               // (1)
template <class UInt, std::size_t N>
  constexpr see_below mul(const std::array<UInt, N>& i, UInt j) noexcept;   // (2)
template <class UInt, std::size_t N>
  constexpr see_below mul(UInt i, const std::array<UInt, N>& j) noexcept;   // (3)
template <class UInt, std::size_t M, std::size_t N>
  constexpr see_below mul(
    const std::array<UInt, M>& i, const std::array<UInt, N>& j) noexcept;   // (4)
```

_Requires:_ `UInt` shall be an unsigned integer type.

_Returns:_ The product of `i` and `j` possibly represented as a big endian sequence of `UInt` words. The return type of (2) and (3) is `std::array<UInt, N + 1>` if `N` is not `0`, `UInt` otherwise. The return type of (4) is `std::array<UInt, M + N>` if neither `M` nor `N` is `0`, `UInt` otherwise.

_Remark:_ (1) shall not participate in overload resolution unless `UInt` is an integer type.

## For developers

Roka has a small test in `src_test` directory.
The test can be built with `cmake -S . -B build -DROKA_BUILD_TESTS=1` in the root directory.
No testing frameworks are employed.
