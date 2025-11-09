/**
 * @file CTSP_instance.cpp
 * @brief Implementation of the Consistent TSP instance class
 */

#include "CTSP_instance.hpp"
#include "TSPLIB_instance.hpp"

namespace CTSP
{
    /**
     * @brief Default constructor
     * Initializes a CTSP instance with default values:
     * - Empty T_ vector
     * - max_distance_ set to a very large value (effectively unbounded)
     * - Empty optimal_values_ vector
     */
    instance::instance(void) : PTSP::instance(), T_(0), max_distance_(1E100), optimal_values_()
    {
    }

    /**
     * @brief Constructor that reads from file
     * @param input_file Path to the CTSP instance file
     * @param log_file Path for logging the reading process
     */
    instance::instance(const string &input_file, const string &log_file) : PTSP::instance(), T_(0), max_distance_(1E100), optimal_values_()
    {
        read(input_file, log_file);
    }

    /**
     * @brief Destructor
     */
    instance::~instance(void)
    {
    }

    /**
     * @brief Reads a CTSP instance from a TSPLIB-formatted file
     * @param input_file Path to the instance file
     * @param log_file Path to the log file
     * 
     * This method:
     * 1. Reads the instance using TSPLIB parser
     * 2. Extracts instance metadata (name, type, comment)
     * 3. Loads customer demands per day
     * 4. Sets maximum distance and time differential constraints
     * 5. Loads optimal/best-known values
     * 6. Validates triangle inequality and symmetry
     */
    void instance::read(const string &input_file, const string &log_file)
    {
        // Use TSPLIB parser to read the file
        TSP::TSPLIB_instance tsplib_instance;

        tsplib_instance.read(input_file, log_file);

        // Extract instance metadata
        id_ = tsplib_instance.get_instance_name();
        type_ = tsplib_instance.get_instance_type();
        comment_ = tsplib_instance.get_instance_comment();

        // Number of customers (excluding depot)
        n_customers_ = tsplib_instance.get_dimension() - 1;

        // Customer demands per day
        demands_ = tsplib_instance.get_demands();

        // Maximum distance constraint per day
        max_distance_ = tsplib_instance.get_max_distance();

        // Number of planning periods/days
        n_days_ = tsplib_instance.get_num_days();

        // Maximum allowable time differential (T) for each customer
        T_.resize(n_customers_);

        for (size_t i{0}; i < n_customers_; i++)
        {
            T_[i] = tsplib_instance.get_maximum_allowable_differencial() ;
        }

        // Optimal/best-known values [no waiting, with waiting]
        optimal_values_ = tsplib_instance.get_optimal_values();

        // Validate distance matrix properties
        triangle_inequality_ = check_triangle_inequality_();

        if (!triangle_inequality_)
        {
            cerr << "Warning: Triangle inequality violated" << endl;
        }

        symmetry_ = check_symmetry_();

        if (!symmetry_)
        {
            cerr << "Warning: Distances are not symmetric" << endl;
        }
    }

    /**
     * @brief Counts total number of customer operations
     * @return Total number of (customer, day) pairs where demand > 0
     * 
     * A customer operation is defined as any customer-day combination
     * where the customer has a positive demand on that day.
     */
    size_t instance::get_n_customer_operations(void) const
    {
        size_t n_customer_operations{0};

        const size_t n_locations{demands_.size()};

        for (size_t i{0}; i < n_locations; i++)
        {
            for (size_t j{0}; j < n_days_; j++)
            {
                if (demands_[i][j] > 0)
                {
                    n_customer_operations++;
                }
            }
        }
        return n_customer_operations;
    }

    /**
     * @brief Writes instance summary as a formatted line to output stream
     * @param os Output stream to write to
     * @return Reference to the output stream
     * 
     * Output format (tab-separated):
     * instance_id | T | max_distance | n_days | n_customers | 
     * n_operations | optimal_no_wait | optimal_with_wait
     */
    ostream &instance::write_line(ostream &os) const
    {
        const size_t n_customer_operations{get_n_customer_operations()};

        os << setw(20) << left << id_ << '\t';
        os << setw(5) << right << T_[0] << '\t';
        os << setw(5) << max_distance_ << '\t';
        os << setw(5) << n_days_ << '\t';
        os << setw(5) << n_customers_ << '\t';
        os << setw(5) << n_customer_operations << '\t';
        os << setw(9) << fixed << setprecision(1) << optimal_values_[0] << '\t';
        os << setw(9) << fixed << setprecision(1) << optimal_values_[1] << '\t';
        return os;
    }

}