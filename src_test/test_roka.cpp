/*
 * These codes are available under CC0 1.0 Universal.
 * https://creativecommons.org/publicdomain/zero/1.0/deed
 */

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include <roka/roka.hpp>

using namespace roka;

int main()
{
    // add_1_1
    {
        constexpr std::uint32_t n = 0xffffffff;
        constexpr auto sum = add(n, n);
        static_assert(sum.size() == 2);
        static_assert(sum[0] == 0x00000001);
        static_assert(sum[1] == 0xfffffffe);
    }

    // add_n_1, add_1_n
    {
        constexpr std::array<std::uint32_t, 2> n = { 0xffffffff, 0xfffffffe };
        constexpr auto sum = add(n, 0xffffffff);
        static_assert(sum.size() == 3);
        static_assert(sum[0] == 0x00000001);
        static_assert(sum[1] == 0x00000000);
        static_assert(sum[2] == 0xfffffffd);

        const auto sum2 = add(0xffffffff, n);
        if (sum2 != sum) {
            std::cout << "Test failed" << std::endl;
            throw std::logic_error("Test failed");
        }
    }

    // add_m_n
    {
        constexpr std::array<std::uint32_t, 2> m = { 0xffffffff, 0xffffffff };
        constexpr std::array<std::uint32_t, 2> n = { 0xffffffff, 0xfffffffe };
        constexpr auto sum = add(m, n);
        static_assert(sum.size() == 3);
        static_assert(sum[0] == 0x00000001);
        static_assert(sum[1] == 0xffffffff);
        static_assert(sum[2] == 0xfffffffd);
    }

    // mul_1_1, mul_n_1, mul_1_n
    {
        constexpr std::uint32_t m = 0xfffffffe;
        constexpr std::uint32_t n = 0xffffffff;
        constexpr auto mn = mul(m, n);
        static_assert(mn.size() == 2);
        static_assert(mn[0] == 0xfffffffd);
        static_assert(mn[1] == 0x00000002);

        constexpr auto mnn = mul(mn, n);
        static_assert(mnn.size() == 3);
        static_assert(mnn[0] == 0xfffffffc);
        static_assert(mnn[1] == 0x00000004);
        static_assert(mnn[2] == 0xfffffffe);

        const auto mnn2 = mul(n, mn);
        if (mnn2 != mnn) {
            std::cout << "Test failed" << std::endl;
            throw std::logic_error("Test failed");
        }
    }

    // mul_m_n
    {
        constexpr std::array<std::uint32_t, 2> m = { 0xffffffff, 0xffffffff };
        constexpr std::array<std::uint32_t, 2> n = { 0xffffffff, 0xfffffffe };
        constexpr auto prod = mul(m, n);
        static_assert(prod.size() == 4);
        static_assert(prod[0] == 0xffffffff);
        static_assert(prod[1] == 0xfffffffd);
        static_assert(prod[2] == 0x00000000);
        static_assert(prod[3] == 0x00000002);

        // We do the following test at runtime
        const std::array<std::uint32_t, 2> l = { 0xffffffff, 0xfffffffd };
        const auto prod2 = mul(l, prod);
        const std::array<std::uint32_t, 6> x =
            { 0xffffffff, 0xfffffffa, 0x00000000,
              0x0000000a, 0xffffffff, 0xfffffffa };
        if (prod2 != x) {
            std::cout << "Test failed" << std::endl;
            throw std::logic_error("Test failed");
        }
    }

    // zero sized arrays are treated as if they represent zero
    {
        constexpr auto sum = add(std::array<unsigned, 0>(), 1u);
        static_assert(sum == 1u);
    }
    {
        constexpr auto sum = add(1u, std::array<unsigned, 0>());
        static_assert(sum == 1u);
    }
    {
        constexpr auto sum = add(
            std::array<unsigned, 2>{ 1u, 1u }, std::array<unsigned, 0>());
        static_assert(sum.size() == 2);
        static_assert(sum[0] == 1u);
        static_assert(sum[1] == 1u);
    }
    {
        constexpr auto sum = add(
            std::array<unsigned, 0>(), std::array<unsigned, 0>{});
        static_assert(sum == 0);
    }
    {
        constexpr auto sum = add(
            std::array<unsigned, 0>(), std::array<unsigned, 2>{ 1u, 1u });
        static_assert(sum.size() == 2);
        static_assert(sum[0] == 1u);
        static_assert(sum[1] == 1u);
    }
    {
        constexpr auto prod = mul(1u, std::array<unsigned, 0>());
        static_assert(prod == 0u);
    }
    {
        constexpr auto prod = mul(std::array<unsigned, 0>(), 1u);
        static_assert(prod == 0u);
    }
    {
        constexpr auto prod = mul(
            std::array<unsigned, 2>{ 1u, 1u }, std::array<unsigned, 0>());
        static_assert(prod == 0u);
    }
    {
        constexpr auto prod = mul(
            std::array<unsigned, 0>(), std::array<unsigned, 2>{ 1u, 1u });
        static_assert(prod == 0u);
    }

    std::cout << "OK" << std::endl;
}
