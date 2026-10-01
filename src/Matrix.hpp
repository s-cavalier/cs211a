#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <algorithm>
#include <cstddef>
#include <concepts>
#include <array>
#include <initializer_list>
#include <ostream>
#include <sstream>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <poet/poet.hpp>

#define TAG_TYPE(name) struct name##_t {}; \
constexpr inline name##_t name = {}

TAG_TYPE(Zeros);
TAG_TYPE(Eye);
TAG_TYPE(Ones);

#define POETSWITCH(regular_case, poet_case) if constexpr (!UsePoet) \
regular_case \
else \
poet_case

template <class T, size_t Rows, size_t Cols = Rows, bool UsePoet = false>
    requires (Rows > 0 && Cols > 0)
class Matrix
{
    std::array<T, Rows * Cols> m_data{};

public:
    constexpr Matrix() noexcept = default;
    constexpr Matrix(std::initializer_list<T> il)
    {
        if (il.size() != Rows * Cols)
        {
            throw std::invalid_argument("Matrix initializer size does not match its dimensions");
        }
        std::copy(il.begin(), il.end(), m_data.begin());
    }

    constexpr Matrix(std::initializer_list<std::initializer_list<T>> il)
    {
        if (il.size() != Rows)
        {
            throw std::invalid_argument("Matrix initializer row count does not match its dimensions");
        }

        size_t ptr = 0;
        for (const auto& row : il)
        {
            if (row.size() != Cols)
            {
                throw std::invalid_argument("Matrix initializer column count does not match its dimensions");
            }
            std::copy(row.begin(), row.end(), m_data.begin() + ptr);
            ptr += Cols;
        }
    }

    constexpr Matrix(Zeros_t) noexcept
    {
        POETSWITCH(
            {
                std::fill_n(m_data.begin(), Rows * Cols, T{0});
            },
            {
                poet::static_for<Rows>([&](auto I)
                {
                    poet::static_for<Cols>([&](auto J)
                    {
                        m_data[I * Cols + J] = T{0};
                    });
                });
            }
        );

    }

    constexpr Matrix(Ones_t) noexcept
    {
        POETSWITCH(
            {
                std::fill_n(m_data.begin(), Rows * Cols, T{1});
            },
            {
                poet::static_for<Rows>([&](auto I)
                {
                    poet::static_for<Cols>([&](auto J)
                    {
                        m_data[I * Cols + J] = T{1};
                    });
                });
            }
        );

    }

    constexpr Matrix(Eye_t) noexcept requires (Rows == Cols)
    {
        POETSWITCH(
            {
                for (size_t i = 0; i < Rows; ++i)
                {
                    for (size_t j = 0; j < Cols; ++j)
                    {
                        if (i == j) [[unlikely]]
                        {
                            m_data[i * Cols + j] = T{1};
                            continue;
                        }

                        m_data[i * Cols + j] = T{0};
                    }
                }
            },
            {
                poet::static_for<Rows>([&](auto I)
                {
                    poet::static_for<Cols>([&](auto J)
                    {
                        if constexpr (I == J) 
                        {
                            m_data[I * Cols + J] = T{1};
                            return;
                        }

                        m_data[I * Cols + J] = T{0};
                    });
                });
            }
        );

    }

    constexpr size_t rows() const
    {
        return Rows;
    }

    constexpr size_t cols() const
    {
        return Cols;
    }

    constexpr size_t size() const
    {
        return Rows * Cols;
    }

    constexpr T* data()
    {
        return m_data.data();
    }

    constexpr const T* data() const
    {
        return m_data.data();
    }

    constexpr std::span<T, Rows * Cols> span()
    {
        return m_data;
    }

    constexpr std::span<const T, Rows * Cols> span() const
    {
        return m_data;
    }

    constexpr std::span<T, Cols> operator[](size_t i)
    {
        return std::span<T, Cols>(m_data.data() + i * Cols, Cols);
    }

    constexpr std::span<const T, Cols> operator[](size_t i) const
    {
        return std::span<const T, Cols>(m_data.data() + i * Cols, Cols);
    }

    constexpr T& operator[](size_t i, size_t j)
    {
        return m_data[i * Cols + j];
    }

    constexpr const T& operator[](size_t i, size_t j) const
    {
        return m_data[i * Cols + j];
    }

    std::string pretty_string() const
    {
        std::ostringstream out;
        out << '[';
        for (size_t i = 0; i < Rows; ++i)
        {
            if (i != 0) out << ",\n ";
            out << '[';
            for (size_t j = 0; j < Cols; ++j)
            {
                if (j != 0) out << ", ";
                out << (*this)[i, j];
            }
            out << ']';
        }
        out << ']';
        return out.str();
    }

    friend std::ostream& operator<<(std::ostream& out, const Matrix& matrix)
    {
        return out << matrix.pretty_string();
    }

    template <size_t OtherRows, size_t OtherCols, bool OtherUsePoet>
    constexpr bool operator==(const Matrix<T, OtherRows, OtherCols, OtherUsePoet>& other) const
    {
        if constexpr (OtherRows != Rows || OtherCols != Cols)
        {
            return false;
        }

        return std::equal(m_data.begin(), m_data.end(), other.m_data.begin());
    }

    template <size_t OtherRows, size_t OtherCols, bool OtherUsePoet>
        requires (Rows == OtherRows && Cols == OtherCols)
    constexpr Matrix& operator+=(const Matrix<T, OtherRows, OtherCols, OtherUsePoet>& rhs)
    {
        if constexpr (!UsePoet)
        {
            for (size_t i = 0; i < Rows; ++i)
            {
                for (size_t j = 0; j < Cols; ++j)
                {
                    (*this)[i, j] += rhs[i, j];
                }
            }
        }
        else
        {
            poet::static_for<Rows>([&](auto I)
            {
                poet::static_for<Cols>([&](auto J)
                {
                    (*this)[I, J] += rhs[I, J];
                });
            });
        }
        return *this;
    }

    constexpr Matrix operator-() const
    {
        Matrix ret;
        POETSWITCH(
            {
                for (size_t i = 0; i < Rows * Cols; ++i)
                {
                    ret.m_data[i] = -m_data[i];
                }
            },
            {
                poet::static_for<Rows * Cols>([&](auto I)
                {
                    ret.m_data[I] = -m_data[I];
                });
            })

        return ret;
    }

    template <size_t OtherRows, size_t OtherCols, bool OtherUsePoet>
        requires (Rows == OtherRows && Cols == OtherCols)
    constexpr Matrix& operator-=(const Matrix<T, OtherRows, OtherCols, OtherUsePoet>& rhs)
    {
        if constexpr (!UsePoet)
        {
            for (size_t i = 0; i < Rows; ++i)
            {
                for (size_t j = 0; j < Cols; ++j)
                {
                    (*this)[i, j] -= rhs[i, j];
                }
            }
        }
        else
        {
            poet::static_for<Rows>([&](auto I)
            {
                poet::static_for<Cols>([&](auto J)
                {
                    (*this)[I, J] -= rhs[I, J];
                });
            });
        }
        return *this;
    }

    template <size_t OtherRows, size_t OtherCols, bool OtherUsePoet>
        requires (Rows == OtherRows && Cols == OtherCols)
    constexpr Matrix operator+(const Matrix<T, OtherRows, OtherCols, OtherUsePoet>& rhs) const
    {
        Matrix result;
        if constexpr (!UsePoet)
        {
            for (size_t i = 0; i < Rows; ++i)
            {
                for (size_t j = 0; j < Cols; ++j)
                {
                    result[i, j] = (*this)[i, j] + rhs[i, j];
                }
            }
        }
        else
        {
            poet::static_for<Rows>([&](auto I)
            {
                poet::static_for<Cols>([&](auto J)
                {
                    result[I, J] = (*this)[I, J] + rhs[I, J];
                });
            });
        }
        return result;
    }

    template <size_t OtherRows, size_t OtherCols, bool OtherUsePoet>
        requires (Rows == OtherRows && Cols == OtherCols)
    constexpr Matrix operator-(const Matrix<T, OtherRows, OtherCols, OtherUsePoet>& rhs) const
    {
        Matrix result;
        if constexpr (!UsePoet)
        {
            for (size_t i = 0; i < Rows; ++i)
            {
                for (size_t j = 0; j < Cols; ++j)
                {
                    result[i, j] = (*this)[i, j] - rhs[i, j];
                }
            }
        }
        else
        {
            poet::static_for<Rows>([&](auto I)
            {
                poet::static_for<Cols>([&](auto J)
                {
                    result[I, J] = (*this)[I, J] - rhs[I, J];
                });
            });
        }
        return result;
    }

    template <size_t OtherRows, size_t OtherCols, bool OtherUsePoet>
        requires (Cols == OtherRows)
    constexpr Matrix<T, Rows, OtherCols, UsePoet>
    operator*(const Matrix<T, OtherRows, OtherCols, OtherUsePoet>& rhs) const
    {
        Matrix<T, Rows, OtherCols, UsePoet> result;
        if constexpr (!UsePoet)
        {
            for (size_t i = 0; i < Rows; ++i)
            {
                for (size_t j = 0; j < OtherCols; ++j)
                {
                    T sum{};
                    for (size_t k = 0; k < Cols; ++k)
                    {
                        sum += (*this)[i, k] * rhs[k, j];
                    }
                    result[i, j] = sum;
                }
            }
        }
        else
        {
            poet::static_for<Rows>([&](auto I)
            {
                poet::static_for<OtherCols>([&](auto J)
                {
                    T sum{};
                    poet::static_for<Cols>([&](auto K)
                    {
                        sum += (*this)[I, K] * rhs[K, J];
                    });
                    result[I, J] = sum;
                });
            });
        }
        return result;
    }

    constexpr Matrix<T, Cols, Rows, UsePoet> transpose() const
    {
        Matrix<T, Cols, Rows, UsePoet> result;
        if constexpr (!UsePoet)
        {
            for (size_t i = 0; i < Rows; ++i)
            {
                for (size_t j = 0; j < Cols; ++j)
                {
                    result[j, i] = (*this)[i, j];
                }
            }
        }
        else
        {
            poet::static_for<Rows>([&](auto I)
            {
                poet::static_for<Cols>([&](auto J)
                {
                    result[J, I] = (*this)[I, J];
                });
            });
        }
        return result;
    }

    constexpr auto inverse() const requires (Rows == Cols)
    {
        using WorkT = std::conditional_t<std::integral<T>, double, T>;
        using WorkMatrix = Matrix<WorkT, Rows, Cols, UsePoet>;
        WorkMatrix work;
        WorkMatrix result(Eye);

        auto reduce = [](WorkT value) constexpr -> WorkT
        {
            if constexpr (requires { value.simplify(); })
            {
                return value.simplify();
            }
            else
            {
                return value;
            }
        };

        for (size_t i = 0; i < Rows; ++i)
        {
            for (size_t j = 0; j < Cols; ++j)
            {
                work[i, j] = reduce(static_cast<WorkT>((*this)[i, j]));
            }
        }

        auto each_column = [&](auto&& action)
        {
            if constexpr (UsePoet)
            {
                poet::static_for<Cols>([&](auto J) { action(static_cast<size_t>(J)); });
            }
            else
            {
                for (size_t j = 0; j < Cols; ++j) action(j);
            }
        };

        const WorkT zero{0};
        for (size_t pivot = 0; pivot < Rows; ++pivot)
        {
            size_t pivot_row = pivot;
            if constexpr (std::floating_point<WorkT>)
            {
                auto magnitude = [](WorkT value) { return value < WorkT{0} ? -value : value; };
                for (size_t row = pivot + 1; row < Rows; ++row)
                {
                    if (magnitude(work[row, pivot]) > magnitude(work[pivot_row, pivot]))
                    {
                        pivot_row = row;
                    }
                }
            }
            else
            {
                while (pivot_row < Rows && work[pivot_row, pivot] == zero) ++pivot_row;
            }

            if (pivot_row == Rows || work[pivot_row, pivot] == zero)
            {
                throw std::domain_error("Matrix is singular");
            }

            if (pivot_row != pivot)
            {
                each_column([&](size_t j)
                {
                    std::swap(work[pivot, j], work[pivot_row, j]);
                    std::swap(result[pivot, j], result[pivot_row, j]);
                });
            }

            const WorkT pivot_value = work[pivot, pivot];
            each_column([&](size_t j)
            {
                work[pivot, j] = reduce(work[pivot, j] / pivot_value);
                result[pivot, j] = reduce(result[pivot, j] / pivot_value);
            });

            for (size_t row = 0; row < Rows; ++row)
            {
                if (row == pivot) continue;
                const WorkT factor = work[row, pivot];
                if (factor == zero) continue;
                each_column([&](size_t j)
                {
                    work[row, j] = reduce(work[row, j] - factor * work[pivot, j]);
                    result[row, j] = reduce(result[row, j] - factor * result[pivot, j]);
                });
            }
        }

        return result;
    }

};

#undef TAG_TYPE
#undef POETSWITCH

#endif
