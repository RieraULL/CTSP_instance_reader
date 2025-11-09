/**
 * @file PTSP_instance.hpp
 * @brief Periodic Traveling Salesman Problem (PTSP) instance definition
 * 
 * This file defines the base instance class for the Periodic TSP,
 * which serves as a parent class for CTSP instances.
 * 
 * The PTSP extends the classical TSP by considering:
 * - Multiple customers with varying demands
 * - Multiple days/periods of planning
 * - Customer demands that may vary by day
 */

#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <utility>

#include "matrix.hpp"

using namespace std;

namespace PTSP
{
    /**
     * @class instance
     * @brief Base class for Periodic TSP instances
     * 
     * This class provides the fundamental data structures and operations
     * for TSP-related problems with Periodic or time-varying demands.
     */
    class instance
    {
    protected:
        /// Instance identifier/name
        string id_;
        
        /// Instance comment (may contain metadata or solution values)
        string comment_;
        
        /// Instance type description
        string type_;

        /// Number of customers (excluding depot)
        size_t n_customers_;
        
        /// Number of planning days/periods
        size_t n_days_;

        /// Distance matrix (indexed from 1 to dimension)
        GOMA::matrix<double> distances_;

        /// Flag indicating if triangle inequality holds
        bool triangle_inequality_;
        
        /// Flag indicating if distance matrix is symmetric
        bool symmetry_;

        /// Customer demands: demands_[customer][day] = demand value
        vector<vector<int>> demands_;

    public:
        /**
         * @brief Default constructor
         */
        instance(void);
        
        /**
         * @brief Virtual destructor
         */
        virtual ~instance(void);

        /**
         * @brief Check if triangle inequality property holds
         * @return true if d(i,j) <= d(i,k) + d(k,j) for all i,j,k
         */
        inline bool triangle_inequality(void) const
        {
            return triangle_inequality_;
        }

        /**
         * @brief Check if distance matrix is symmetric
         * @return true if d(i,j) == d(j,i) for all i,j
         */
        inline bool symmetry(void) const
        {
            return symmetry_;
        }

        /**
         * @brief Get instance name
         * @return Instance identifier string
         */
        inline const string &get_instance_name(void) const
        {
            return id_;
        }

        /**
         * @brief Get instance comment
         * @return Comment string (may contain optimal values or metadata)
         */
        inline const string &get_instance_comment(void) const
        {
            return comment_;
        }

        /**
         * @brief Get instance type
         * @return Type description string
         */
        inline const string &get_instance_type(void) const
        {
            return type_;
        }

        /**
         * @brief Get distance matrix
         * @return Reference to the distance matrix (1-indexed)
         */
        inline const GOMA::matrix<double> &get_distances(void) const
        {
            return distances_;
        }

        /**
         * @brief Get number of planning days/periods
         * @return Number of days
         */
        inline const size_t &get_n_days(void) const
        {
            return n_days_;
        }

        /**
         * @brief Get number of customers (excluding depot)
         * @return Number of customers
         */
        inline size_t get_n_customers(void) const
        {
            return n_customers_;
        }

        /**
         * @brief Get customer demands matrix
         * @return 2D vector where demands[i][j] = demand of customer i on day j
         */
        inline const vector<vector<int>> &get_demands(void) const
        {
            return demands_;
        }

    protected:
        /**
         * @brief Verify triangle inequality property
         * @return true if triangle inequality holds for all triples
         * 
         * Checks that for all distinct i,j,k:
         * distance(i,j) <= distance(i,k) + distance(k,j)
         */
        bool check_triangle_inequality_(void) const;
        
        /**
         * @brief Verify symmetry of distance matrix
         * @return true if distance(i,j) == distance(j,i) for all i,j
         * 
         * Allows small tolerance (1E-2) for floating point comparisons
         */
        bool check_symmetry_(void) const;
    };
}