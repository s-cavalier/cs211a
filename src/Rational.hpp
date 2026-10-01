#ifndef RATIONAL_HPP
#define RATIONAL_HPP
#include <cassert>
#include <concepts>
#include <numeric>
#include <ostream>
#include <sstream>
#include <string>

template <class T>
class Rational
{
    T m_n;
    T m_d;

public:
    constexpr Rational() : m_n(0), m_d(1) {}

    constexpr Rational(const std::convertible_to<T> auto& n) : m_n(n), m_d(1) {}

    constexpr auto operator<=>(const Rational& other) const = default;

    constexpr Rational(const T& num, const T& den) : m_n(num), m_d(den)
    {
        assert(m_d != 0);
    }

    constexpr const T& numerator() const
    {
        return m_n;
    }

    constexpr const T& denominator() const
    {
        return m_d;
    }

    constexpr bool operator==(const Rational& rhs) const
    {
        return m_n * rhs.m_d == rhs.m_n * m_d;
    }

    constexpr Rational simplify() const requires (std::integral<T> && !std::same_as<T, bool>)
    {
        const T divisor = std::gcd(m_n, m_d);
        T num = m_n / divisor;
        T den = m_d / divisor;

        if constexpr (std::signed_integral<T>)
        {
            if (den < 0)
            {
                num = -num;
                den = -den;
            }
        }

        return {num, den};
    }

    std::string pretty_string() const
    {
        Rational value = *this;
        if constexpr (std::integral<T> && !std::same_as<T, bool>)
        {
            value = simplify();
        }

        std::ostringstream out;
        out << value.m_n;
        if (value.m_d != T{1})
        {
            out << '/' << value.m_d;
        }
        return out.str();
    }

    friend std::ostream& operator<<(std::ostream& out, const Rational& value)
    {
        return out << value.pretty_string();
    }

    constexpr Rational operator+(const Rational& rhs) const
    {
        return {m_n * rhs.m_d + rhs.m_n * m_d, m_d * rhs.m_d};
    }

    constexpr Rational operator-(const Rational& rhs) const
    {
        return {m_n * rhs.m_d - rhs.m_n * m_d, m_d * rhs.m_d};
    }

    constexpr Rational operator*(const Rational& rhs) const
    {
        return {m_n * rhs.m_n, m_d * rhs.m_d};
    }

    constexpr Rational operator/(const Rational& rhs) const
    {
        return {m_n * rhs.m_d, m_d * rhs.m_n};
    }

    constexpr Rational& operator+=(const Rational& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    constexpr Rational& operator-=(const Rational& rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    constexpr Rational& operator*=(const Rational& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    constexpr Rational& operator/=(const Rational& rhs)
    {
        *this = *this / rhs;
        return *this;
    }

    constexpr Rational operator-() const
    {
        return Rational{ -m_n, m_d };
    }

    template <std::floating_point F>
    constexpr operator F() const
    {
        return static_cast<F>(m_n) / static_cast<F>(m_d);
    }

};

#endif
