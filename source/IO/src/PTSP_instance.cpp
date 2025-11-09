/**
 * @file PTSP_instance.cpp
 * @brief Implementation of the Probabilistic TSP instance class
 */

#include "PTSP_instance.hpp"
#include <cmath>

namespace PTSP
{
    /**
     * @brief Default constructor
     * Initializes all members to default/empty values
     */
    instance::instance(void) : id_(""), n_customers_(0), n_days_(0), distances_(), triangle_inequality_(false), symmetry_(false)
    {
    }

    /**
     * @brief Destructor
     */
    instance::~instance(void)
    {
    }

    /**
     * @brief Checks triangle inequality property for the distance matrix
     * @return true if triangle inequality holds for all node triples
     * 
     * The triangle inequality states that for any three nodes i, j, k:
     * d(i,j) <= d(i,k) + d(k,j)
     * 
     * This property is important for many TSP algorithms and guarantees
     * that direct routes are never longer than indirect routes.
     * 
     * A tolerance of 1 + 1E-2 is used to account for rounding in distance calculations.
     */
    bool instance::check_triangle_inequality_(void) const
    {
        const size_t n_locations{distances_.get_n_rows()};

        // Check all possible triples (i, j, k)
        for (size_t i{0}; i < n_locations; i++)
        {
            for (size_t j{0}; j < n_locations; j++)
            {
                for (size_t k{0}; k < n_locations; k++)
                {
                    // Only check distinct nodes
                    if (i != j && j != k && i != k)
                    {
                        const double dist_ij{distances_(i + 1, j + 1)};
                        const double dist_ik{distances_(i + 1, k + 1)};
                        const double dist_kj{distances_(k + 1, j + 1)};

                        const double dist_ik_kj{ dist_ik + dist_kj };

                        // Check if going through k is significantly shorter than direct i->j
                        // (with tolerance for rounding errors)
                        if (dist_ik_kj < dist_ij - (1 + 1E-2))
                        {
                            return false;
                        }

                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Checks if the distance matrix is symmetric
     * @return true if d(i,j) == d(j,i) for all i,j (within tolerance)
     * 
     * Symmetry is a common property in TSP instances where the cost of
     * traveling from i to j equals the cost from j to i.
     * 
     * Uses tolerance of 1E-2 for floating point comparison.
     */
    bool instance::check_symmetry_(void) const
    {
        const size_t n_locations{distances_.get_n_rows()};

        for (size_t i{0}; i < n_locations; i++)
        {
            for (size_t j{0}; j < n_locations; j++)
            {
                if (i != j)
                {
                    const double dist_ij{distances_(i + 1, j + 1)};
                    const double dist_ji{distances_(j + 1, i + 1)};

                    // Check symmetry with tolerance for floating point errors
                    if (fabs(dist_ij - dist_ji) > 1E-2)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }
}