#include "Matrix.hpp"
#include "Rational.hpp"

#include <array>
#include <complex>
#include <cstdint>
#include <iomanip>
#include <iostream>

using NumT = std::complex<Rational<std::int32_t>>;
using Mat = Matrix<NumT, 2, 2, true>;

constexpr Mat I(Eye);
constexpr Mat j{NumT{0, 1}, NumT{}, NumT{}, NumT{0, -1}};
constexpr Mat k{NumT{}, NumT{0, 1}, NumT{0, 1}, NumT{}};
constexpr Mat l{NumT{}, NumT{-1, 0}, NumT{1, 0}, NumT{}};

struct Relation
{
    const char* name;
    bool holds;
};

constexpr std::array relations{
    Relation{"j*j = -I", j * j == -I},
    Relation{"k*k = -I", k * k == -I},
    Relation{"l*l = -I", l * l == -I},
    Relation{"j*k = l", j * k == l},
    Relation{"k*l = j", k * l == j},
    Relation{"l*j = k", l * j == k},
    Relation{"k*j = -l", k * j == -l},
    Relation{"l*k = -j", l * k == -j},
    Relation{"j*l = -k", j * l == -k},
};

static_assert([]
{
    for (const Relation& relation : relations)
    {
        if (!relation.holds) return false;
    }
    return true;
}());

constexpr Matrix<Rational<int32_t>, 4, 4, true> antUpdate
{
    { Rational<int32_t>{4, 10}, Rational<int32_t>{0, 10}, Rational<int32_t>{0, 10}, Rational<int32_t>{2, 10} },
    { Rational<int32_t>{0, 10}, Rational<int32_t>{4, 10}, Rational<int32_t>{3, 10}, Rational<int32_t>{2, 10} },
    { Rational<int32_t>{0, 10}, Rational<int32_t>{3, 10}, Rational<int32_t>{4, 10}, Rational<int32_t>{2, 10} },
    { Rational<int32_t>{6, 10}, Rational<int32_t>{3, 10}, Rational<int32_t>{3, 10}, Rational<int32_t>{4, 10} },
};

constexpr auto antInverseUpdate = antUpdate.inverse();

constexpr Matrix<Rational<int32_t>, 4, 1, true> observedAnts{ 12, 25, 26, 37 };

constexpr auto antsPrev = antInverseUpdate * observedAnts;

int main()
{
    std::cout << std::boolalpha << std::left;
    for (const Relation& relation : relations)
    {
        std::cout << std::setw(16) << relation.name << " : " << relation.holds << '\n';
    }

    std::cout << "ants in rooms 1, 2, 3, 4:\n" << antsPrev.transpose().pretty_string() << '\n';
}
