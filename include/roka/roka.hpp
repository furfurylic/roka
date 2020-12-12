/*
 * These codes are available under CC0 1.0 Universal.
 * https://creativecommons.org/publicdomain/zero/1.0/deed
 */

#ifndef ROKA_GUARD_D78D03EA_BDEF_4829_88E2_A4A88AF34699
#define ROKA_GUARD_D78D03EA_BDEF_4829_88E2_A4A88AF34699

#include <array>
#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace roka {

namespace detail {

template <std::size_t Offset, class T, std::size_t N,
          class FrontTuple, class BackTuple,
          std::size_t... Is, std::size_t... FrontIs, std::size_t... BackIs>
constexpr auto clip_impl2(
        const FrontTuple& fronts, std::index_sequence<FrontIs...>,
        const std::array<T, N>& a, std::index_sequence<Is...>,
        const BackTuple& backs, std::index_sequence<BackIs...>)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
{
    constexpr std::size_t length =
        sizeof...(FrontIs) + sizeof...(Is) + sizeof...(BackIs);
    return std::array<T, length> { std::get<FrontIs>(fronts)...,
                                   a[Offset + Is]...,
                                   std::get<BackIs>(backs)... };
}

template <std::size_t Offset, class T, std::size_t N,
          class FrontTuple, class BackTuple, std::size_t... Is>
constexpr auto clip_impl(const FrontTuple& fronts,
                         const std::array<T, N>& a, std::index_sequence<Is...>,
                         const BackTuple& backs)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
{
    return clip_impl2<Offset>(
        fronts, std::make_index_sequence<std::tuple_size<FrontTuple>::value>(),
        a, std::index_sequence<Is...>(),
        backs, std::make_index_sequence<std::tuple_size<BackTuple>::value>());
}

// Makes an array that equals { front, a[F], a[F + 1], ..., a[L - 1] }
template <std::size_t F, std::size_t L, class T, std::size_t N>
constexpr auto clip(T front, const std::array<T, N>& a)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
{
    static_assert(F <= L);
    static_assert(L <= N);
    return clip_impl<F>(std::make_tuple(front),
                        a, std::make_index_sequence<L - F>(),
                        std::tuple<>());
}

// Makes an array that equals { a[F], a[F + 1], ..., a[L - 1], back }
template <std::size_t F, std::size_t L, class T, std::size_t N>
constexpr auto clip(const std::array<T, N>& a, T back)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
{
    static_assert(F <= L);
    static_assert(L <= N);
    return clip_impl<F>(std::tuple<>(),
                        a, std::make_index_sequence<L - F>(),
                        std::make_tuple(back));
}

template <class UInt, std::size_t digits>
constexpr UInt fill() noexcept
{
    static_assert(std::is_unsigned_v<UInt>);
    UInt n = 0;
    for (std::size_t i = 0; i < digits; ++i) {
        n <<= 1;
        n |= 1;
    }
    return n;
}

template <class UInt>
constexpr std::tuple<UInt, UInt> hilo(UInt n) noexcept
{
    static_assert(std::is_unsigned_v<UInt>);
    constexpr std::size_t digits = std::numeric_limits<UInt>::digits;
    constexpr std::size_t half_digits = digits / 2;
    static_assert(half_digits * 2 == digits);
    const UInt h = n >> half_digits;
    constexpr UInt l_mask = fill<UInt, half_digits>();
    const UInt l = l_mask & n;
    return std::tuple<UInt, UInt>(h, l);
}

}

template <class UInt>
constexpr auto add(UInt i, UInt j) noexcept
 -> std::enable_if_t<std::is_integral_v<UInt>, std::array<UInt, 2>>
{
    static_assert(std::is_unsigned_v<UInt>);
    constexpr std::size_t digits = std::numeric_limits<UInt>::digits;
    constexpr std::size_t half_digits = digits / 2;
    static_assert(half_digits * 2 == digits);
    const auto [ih, il] = detail::hilo(i);
    const auto [jh, jl] = detail::hilo(j);
    const auto [lh, ll] = detail::hilo(il + jl);
    const auto [hh, hl] = detail::hilo(ih + jh + lh);
    return { hh, ((hl << half_digits) | ll) };
}

namespace detail {

template <std::size_t N, class UInt>
constexpr std::array<UInt, N + 1> add_p(const UInt* i, UInt j)
{
    static_assert(N > 0);
    if constexpr (N == 1) {
        return add(i[0], j);
    } else {
        static_assert(std::is_unsigned_v<UInt>);
        const std::array<UInt, 2> l = add(i[N - 1], j);
        const std::array<UInt, N> h = add_p<N - 1>(i, l[0]);
        return clip<0, N>(h, l[1]);
    }
}

}

template <class UInt, std::size_t N>
constexpr std::conditional_t<N == 0, UInt, std::array<UInt, N + 1>> add(
    [[maybe_unused]] const std::array<UInt, N>& i, UInt j) noexcept
{
    if constexpr (N == 0) {
        return j;
    } else {
        return detail::add_p<N>(i.data(), j);
    }
}

template <class UInt, std::size_t N>
constexpr std::conditional_t<N == 0, UInt, std::array<UInt, N + 1>> add(
    UInt i, [[maybe_unused]] const std::array<UInt, N>& j) noexcept
{
    return add(j, i);
}

namespace detail {

template <std::size_t M, std::size_t N, class UInt>
constexpr std::array<UInt, ((M > N) ? M : N) + 1> add_p(
    const UInt* i, const UInt* j)
{
    static_assert(N > 0);
    if constexpr (M == 1) {
        return add_p<N>(j, i[0]);
    } else if constexpr (N == 1) {
        return add_p<M>(i, j[0]);
    } else {
        static_assert(std::is_unsigned_v<UInt>);
        constexpr std::size_t mn = (M > N) ? M : N;
        const std::array<UInt, 2> l = add(i[M - 1], j[N - 1]);
        const std::array<UInt, mn> h = add_p<M - 1, N - 1>(i, j);
        return clip<1, mn + 1>(add_p<mn>(h.data(), l[0]), l[1]);
    }
}

}

template <class UInt, std::size_t M, std::size_t N>
constexpr
std::conditional_t<(M == 0) && (N == 0),
    UInt,
    std::array<
        UInt,
        ((M > N) ? M : N) + (((M == 0) || (N == 0)) ? 0 : 1)>> add(
    [[maybe_unused]] const std::array<UInt, M>& i,
    [[maybe_unused]] const std::array<UInt, N>& j) noexcept
{
    if constexpr (M == 0) {
        if constexpr (N == 0) {
            return static_cast<UInt>(0);
        } else {
            return j;
        }
    } else if constexpr (N == 0) {
        return i;
    } else {
        return detail::add_p<M, N>(i.data(), j.data());
    }
}

template <class UInt>
constexpr auto mul(UInt i, UInt j) noexcept
 -> std::enable_if_t<std::is_integral_v<UInt>, std::array<UInt, 2>>
{
    static_assert(std::is_unsigned_v<UInt>);
    constexpr std::size_t digits = std::numeric_limits<UInt>::digits;
    constexpr std::size_t half_digits = digits / 2;
    static_assert(half_digits * 2 == digits);
    const auto [ih, il] = detail::hilo(i);
    const auto [jh, jl] = detail::hilo(j);
    auto [lh, ll] = detail::hilo(il * jl);
    auto h = ih * jh;
    {
        const auto [mh, ml] = detail::hilo(ih * jl);
        const auto [ph, pl] = detail::hilo(ml + lh);
        h += ph;
        lh = pl;
        h += mh;
    }
    {
        const auto [mh, ml] = detail::hilo(il * jh);
        const auto [ph, pl] = detail::hilo(ml + lh);
        h += ph;
        lh = pl;
        h += mh;
    }
    return { h, ((lh << half_digits) + ll) };
}

namespace detail {

template <std::size_t N, class UInt>
constexpr std::array<UInt, N + 1> mul_p(const UInt* i, UInt j)
{
    static_assert(N > 0);
    if constexpr (N == 1) {
        return mul(i[0], j);
    } else {
        static_assert(std::is_unsigned_v<UInt>);
        const std::array<UInt, 2> l = mul(i[N - 1], j);
        const std::array<UInt, N> h = mul_p<N - 1>(i, j);
        return clip<1, N + 1>(add(h, l[0]), l[1]);
    }
}

}

template <class UInt, std::size_t N>
constexpr
std::conditional_t<N == 0, UInt, std::array<UInt, N + 1>> mul(
    [[maybe_unused]] const std::array<UInt, N>& i,
    [[maybe_unused]] UInt j) noexcept
{
    if constexpr (N == 0) {
        return static_cast<UInt>(0);
    } else {
        return detail::mul_p<N>(i.data(), j);
    }
}

template <class UInt, std::size_t N>
constexpr
std::conditional_t<N == 0, UInt, std::array<UInt, N + 1>> mul(
    UInt i, const std::array<UInt, N>& j) noexcept
{
    return mul(j, i);
}

namespace detail {

template <std::size_t M, std::size_t N, class UInt>
constexpr std::array<UInt, M + N> mul_p(const UInt* i, const UInt* j)
{
    static_assert(M > 0);
    static_assert(N > 0);
    if constexpr (M == 1) {
        return mul_p<N>(j, i[0]);
    } else if constexpr (N == 1) {
        return mul_p<M>(i, j[0]);
    } else {
        static_assert(std::is_unsigned_v<UInt>);
        const std::array<UInt, M + 1> l = mul_p<M>(i, j[N - 1]);
        const std::array<UInt, M + N - 1> h = mul_p<M, N - 1>(i, j);
        return clip<1, M + N>(add_p<M + N - 1, M>(h.data(), l.data()), l[M]);
    }
}

}

template <class UInt, std::size_t M, std::size_t N>
constexpr
std::conditional_t<(M == 0) || (N == 0), UInt, std::array<UInt, M + N>> mul(
    [[maybe_unused]] const std::array<UInt, M>& i,
    [[maybe_unused]] const std::array<UInt, N>& j) noexcept
{
    if constexpr ((M == 0) || (N == 0)) {
        return static_cast<UInt>(0);
    } else {
        return detail::mul_p<M, N>(i.data(), j.data());
    }
}

}

#endif
