/**
 * @file matrix.hpp
 * @brief Generic 2D matrix template class with 1-based indexing
 * 
 * This file provides a simple yet efficient matrix implementation
 * used throughout the CTSP instance reader for storing distance matrices.
 * 
 * Key features:
 * - 1-based indexing (mathematical convention)
 * - Dynamic resizing
 * - Type-safe template implementation
 * - I/O operators for reading/writing matrices
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <cassert>

#define WIDE 6         ///< Default column width for matrix output
#define PRECISION 1    ///< Default decimal precision for output

using namespace std;

namespace GOMA
{

    /**
     * @class matrix
     * @brief 2D matrix with 1-based indexing
     * @tparam T Element type (typically double or int)
     * 
     * This class provides a mathematical-style matrix with 1-based indexing.
     * Elements are accessed as matrix(i, j) where i,j ∈ [1, dimension].
     * 
     * Internal storage uses a linear array for efficiency.
     */
    template <class T>
    class matrix
    {
    private:
        size_t m_;  ///< Number of rows
        size_t n_;  ///< Number of columns
        T *v_;      ///< Data storage (linear array)

    public:
        /**
         * @brief Default constructor - creates empty matrix
         */
        matrix(void) : m_(0),
                       n_(0),
                       v_(NULL) {}

        /**
         * @brief Constructor with dimensions
         * @param m Number of rows
         * @param n Number of columns
         */
        matrix(int m, int n) : m_(m),
                               n_(n),
                               v_(NULL)
        {
            build();
        }

        /**
         * @brief Constructor with dimensions and initial value
         * @param m Number of rows
         * @param n Number of columns
         * @param data Initial value for all elements
         */
        matrix(int m, int n, T data) : m_(m),
                                       n_(n),
                                       v_(NULL)
        {
            build();

            fill(data);
        }

        /**
         * @brief Copy constructor
         * @param M Matrix to copy
         */
        matrix(const matrix &M) : m_(M.m_),
                                  n_(M.n_),
                                  v_(NULL)
        {
            build();

            copy(M.v_, M.m_, M.n_, v_, m_, n_);
        }

        /**
         * @brief Destructor - frees allocated memory
         */
        virtual ~matrix(void)
        {
            if (v_)
            {
                delete[] v_;
            }
        }

        /**
         * @brief Resize matrix (discards old data)
         * @param m New number of rows
         * @param n New number of columns
         */
        void resize(size_t m, size_t n)
        {
            if (m == m_ && n == n_)
                return;

            m_ = m;
            n_ = n;

            build();

            init(0);
        }

        /**
         * @brief Resize matrix while preserving existing data
         * @param m New number of rows
         * @param n New number of columns
         * 
         * Copies overlapping region from old to new matrix.
         * New elements are uninitialized.
         */
        void resize_and_keep(size_t m, size_t n)
        {
            if (m == m_ && n == n_)
                return;

            T *v;

            build(v, m, n);

            copy(v_, m_, n_, v, m, n);

            v_ = v;

            m_ = m;
            n_ = n;
        }

        /**
         * @brief Copy data from one matrix to another
         * @param v Source data array
         * @param m Source rows
         * @param n Source columns
         * @param w Destination data array
         * @param p Destination rows
         * @param q Destination columns
         * 
         * Copies min(m,p) x min(n,q) elements
         */
        void copy(T *v, size_t m, size_t n, T *&w, size_t p, size_t q)
        {
            const size_t l_m = min(m, p);
            const size_t l_n = min(n, q);

            for (size_t i = 1; i <= l_m; i++)
                for (size_t j = 1; j <= l_n; j++)
                    w[pos(q, i, j)] = v[pos(i, j)];
        }

        /**
         * @brief Initialize all elements to a value
         * @param data Value to set
         */
        void init(T data)
        {

            fill(data);
        }

        /**
         * @brief Fill matrix with a constant value
         * @param data Fill value
         */
        void fill(T data)
        {
            const size_t sz = m_ * n_;

            T *v_ptr{v_};

            for (size_t i{0}; i < sz; i++)
            {
                *v_ptr = data;
                v_ptr++;
            }
        }

        /**
         * @brief Access element by linear index (0-based)
         * @param i Linear index
         * @return Reference to element
         */
        T &operator[](size_t i)
        {
            assert(i < m_ * n_);

            return v_[i];
        }

        /**
         * @brief Access element by linear index (0-based, const version)
         */
        const T &operator[](size_t i) const
        {
            assert(i < m_ * n_);

            return v_[i];
        }

        /**
         * @brief Access element by row and column (1-based)
         * @param i Row index (1-based)
         * @param j Column index (1-based)
         * @return Reference to element at position (i,j)
         */
        T &operator()(size_t i, size_t j)
        {
            assert(i >= 1);
            assert(i <= m_);

            assert(j >= 1);
            assert(j <= n_);

            return v_[pos(i, j)];
        }

        /**
         * @brief Access element with bounds checking
         * @param i Row index (1-based)
         * @param j Column index (1-based)
         * @return Reference to element
         */
        T &at(size_t i, size_t j)
        {
            assert(i >= 1);
            assert(i <= m_);

            assert(j >= 1);
            assert(j <= n_);

            return v_[pos(i, j)];
        }

        /**
         * @brief Access element by row and column (const version)
         */
        const T &operator()(size_t i, size_t j) const
        {
            assert(i >= 1);
            assert(i <= m_);

            assert(j >= 1);
            assert(j <= n_);

            return v_[pos(i, j)];
        }

        /**
         * @brief Access element with bounds checking (const version)
         */
        const T &at(size_t i, size_t j) const
        {
            assert(i >= 1);
            assert(i <= m_);

            assert(j >= 1);
            assert(j <= n_);

            return v_[pos(i, j)];
        }

        /**
         * @brief Assignment operator
         * @param M Matrix to copy from
         * @return Reference to this matrix
         */
        matrix &operator=(const matrix &M)
        {
            if (this == &M)
                return *this;

            if (m_ != M.m_ || n_ != M.n_)
            {
                resize(M.m_, M.n_);
            }

            copy(M.v_, M.m_, M.n_, v_, m_, n_);

            return *this;
        }

        /**
         * @brief Get number of rows
         */
        inline size_t get_m(void) const
        {
            return m_;
        }

        /**
         * @brief Get number of columns
         */
        inline size_t get_n(void) const
        {
            return n_;
        }

        /**
         * @brief Get number of rows (alias)
         */
        inline size_t get_n_rows(void) const
        {
            return m_;
        }

        /**
         * @brief Get number of columns (alias)
         */
        inline size_t get_n_cols(void) const
        {
            return n_;
        }

        /**
         * @brief Compute transpose of this matrix
         * @param M Output matrix (will be resized to n x m)
         */
        void transpose(matrix &M) const
        {
            M.resize(n_, m_);

            for (size_t i = 1; i <= m_; i++)
                for (size_t j = 1; j <= n_; j++)
                    M(j, i) = (*this)(i, j);
        }

        /**
         * @brief Read matrix from input stream
         * @param is Input stream
         * @return Reference to input stream
         */
        istream &read_raw(istream &is)
        {

            for (size_t i = 1; i <= m_; i++)
                for (size_t j = 1; j <= n_; j++)
                    is >> v_[pos(i, j)];

            return is;
        }

        /**
         * @brief Write matrix to output stream
         * @param os Output stream
         * @return Reference to output stream
         * 
         * Large values (> 1E+6) are displayed as "inf"
         */
        ostream &write_raw(ostream &os) const
        {

            for (size_t i = 1; i <= m_; i++)
            {
                for (size_t j = 1; j <= n_; j++)
                {
                    const double val = v_[pos(i, j)];

                    if (val > 1E+6)
                        os << setw(WIDE) << "   inf ";
                    else
                        os << setw(WIDE) << fixed << setprecision(PRECISION) << val << " ";
                }
                os << endl;
            }

            return os;
        }

        /**
         * @brief Convert 2D index to linear index
         * @param i Row (1-based)
         * @param j Column (1-based)
         * @return Linear index (0-based)
         */
        inline int pos(size_t i, size_t j) const
        {
            return (i - 1) * n_ + j - 1;
        }

        /**
         * @brief Convert 2D index to linear index (static version)
         * @param n Number of columns
         * @param i Row (1-based)
         * @param j Column (1-based)
         * @return Linear index (0-based)
         */
        inline int pos(size_t n, size_t i, size_t j)
        {
            return (i - 1) * n + j - 1;
        }

    private:
        /**
         * @brief Allocate memory for matrix data
         */
        void build(void)
        {
            if (v_)
            {
                delete[] v_;
            }

            if (m_ * n_ > 0)
            {
                v_ = new T[m_ * n_];
            }
            else
            {
                v_ = NULL;
            }
        }

        /**
         * @brief Allocate memory for external array
         * @param v Array pointer to allocate
         * @param m Number of rows
         * @param n Number of columns
         */
        void
        build(T *v, size_t m, size_t n)
        {
            if (v)
            {
                delete[] v;
            }

            if (m * n > 0)
            {
                v = new T[m * n];
            }
            else
            {
                v = NULL;
            }
        }
    };
}

/**
 * @brief Input stream operator for matrix
 */
template <class T>
istream &operator>>(istream &is, GOMA::matrix<T> &M)
{
    M.read_raw(is);
    return is;
}

/**
 * @brief Output stream operator for matrix
 */
template <class T>
ostream &operator<<(ostream &os, const GOMA::matrix<T> &M)
{
    M.write_raw(os);
    return os;
}
