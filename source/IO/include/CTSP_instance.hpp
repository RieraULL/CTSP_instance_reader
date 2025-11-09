/**
 * @file CTSP_instance.hpp
 * @brief Consistent Traveling Salesman Problem (CTSP) instance definition
 * 
 * This file defines the instance class for the Consistent Traveling Salesman Problem,
 * which extends the Periodic TSP (PTSP) with consistency constraints.
 * 
 * The CTSP is a variant of the TSP where:
 * - Customers must be visited across multiple days/periods
 * - There is a maximum allowable differential (T) for service times at each customer
 * - Each day has a maximum distance constraint
 * - The solution must maintain consistency in service patterns
 */

#pragma once

#include "PTSP_instance.hpp"

using namespace std;

namespace CTSP
{
    /**
     * @class instance
     * @brief Represents a Consistent TSP problem instance
     * 
     * This class extends PTSP::instance to handle CTSP-specific constraints:
     * - Maximum allowable time differential per customer (T_)
     * - Maximum distance per day constraint
     * - Optimal/best-known solution values for benchmarking
     */
    class instance: public PTSP::instance
    {
    protected:
        /// Maximum allowable time differential per customer (consistency constraint)
        vector<double> T_;
        
        /// Maximum distance/cost allowed per day/period
        double max_distance_;

        /// Optimal or best-known solution values [0: without waiting, 1: with waiting]
        vector<double> optimal_values_;

    public:
        /**
         * @brief Default constructor
         * Initializes an empty CTSP instance
         */
        instance(void);
        
        /**
         * @brief Constructor that reads instance from file
         * @param input_file Path to the instance file (TSPLIB format with CTSP extensions)
         * @param log_file Path to the log file for reading diagnostics
         */
        instance(const string &input_file, const string &log_file);
        
        /**
         * @brief Virtual destructor
         */
        virtual ~instance(void);

        /**
         * @brief Reads a CTSP instance from a file
         * @param input_file Path to the instance file
         * @param log_file Path to the log file for diagnostics
         * 
         * Parses CTSP instance data including:
         * - Distance matrix or coordinates
         * - Customer demands per day
         * - Maximum distance constraint
         * - Maximum allowable differential (T)
         * - Optimal/best-known values
         */
        virtual void read(const string &input_file, const string &log_file);

        /**
         * @brief Get optimal or best-known solution values
         * @return Vector with [0]: optimal without waiting, [1]: optimal with waiting
         */
        inline const vector<double> &get_optimal_values(void) const
        {
            return optimal_values_;
        }

        /**
         * @brief Get maximum distance constraint per day
         * @return Maximum allowed distance/cost per day
         */
        inline const double &get_max_distance(void) const
        {
            return max_distance_;
        }

        /**
         * @brief Get maximum allowable time differential per customer
         * @return Vector of T values (consistency constraints) for each customer
         */
        inline const vector<double> &get_T(void) const
        {
            return T_;
        }

        /**
         * @brief Get customer demands matrix
         * @return 2D vector: demands_[location][day] = demand value
         */
        inline const vector<vector<int>> &get_demands(void) const
        {
            return demands_;
        }

        /**
         * @brief Calculate total number of customer operations across all days
         * @return Count of all (customer, day) pairs where demand > 0
         */
        size_t get_n_customer_operations(void) const;

        /**
         * @brief Write instance summary as a formatted line
         * @param os Output stream
         * @return Reference to the output stream
         * 
         * Outputs: instance_id, T, max_distance, n_days, n_customers, 
         *          n_operations, optimal_value_no_wait, optimal_value_with_wait
         */
        ostream &write_line(ostream &os) const;

    };
}