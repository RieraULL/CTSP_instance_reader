/**
 * @file TSPLIB_instance.hpp
 * @brief TSPLIB format parser for TSP and related problem instances
 * 
 * This file provides a comprehensive parser for the TSPLIB file format,
 * which is a widely-used standard for representing TSP instances.
 * 
 * The parser supports:
 * - Multiple distance calculation methods (Euclidean, Manhattan, Geographical, etc.)
 * - Various edge weight matrix formats (full, upper/lower triangular, etc.)
 * - Extended fields for CTSP/PTSP (demands, time windows, etc.)
 * - Coordinate-based and explicit distance matrix instances
 * 
 * Reference: TSPLIB - http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/
 */

#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include <cmath>
#include <algorithm>
#include "matrix.hpp"

using namespace std;

/// Number of node coordinate types
#define NCTYPE_NUM 3

/// Number of edge weight types (distance calculation methods)
#define WTYPE_NUM 10

/// Number of edge weight formats (matrix storage formats)
#define WFORMAT_NUM 9

/// Number of display data types
#define DTYPE_NUM 3

/// Total number of TSPLIB keywords
#define KEY_NUM 25

namespace TSP
{
    /// Type for coordinate components (x, y)
    typedef double coordItemType;
    
    /// Type for 2D coordinates
    /// Type for 2D coordinates
    typedef pair<coordItemType, coordItemType> coordType;

    /**
     * @class TSPLIB_instance
     * @brief Parser and container for TSPLIB-formatted TSP instances
     * 
     * This class can read and parse TSPLIB files, supporting:
     * - Standard TSP instances
     * - Vehicle Routing Problem (VRP) extensions
     * - CTSP/PTSP extensions (multiple days, demands, constraints)
     * 
     * The class handles:
     * - Automatic distance matrix computation from coordinates
     * - Multiple distance metrics (Euclidean, Manhattan, Geographic, etc.)
     * - Various matrix storage formats
     * - Instance validation and preprocessing
     */
    class TSPLIB_instance
    {
        /// Function pointer type for distance calculation methods
        typedef double (TSPLIB_instance::*distanceType)(const coordType &, const coordType &) const;
        
        /// Function pointer type for section reading methods
        typedef void (TSPLIB_instance::*readType)(istream &is, ostream &os);

    private:
        /// TSPLIB specification keywords
        const vector<string> keywords = {
            /* 0*/ "NAME",                              // Instance name
            /* 1*/ "TYPE",                              // Problem type (TSP, CVRP, etc.)
            /* 2*/ "COMMENT",                           // Comments (may include optimal values)
            /* 3*/ "DIMENSION",                         // Number of nodes
            /* 4*/ "CAPACITY",                          // Vehicle capacity (VRP)
            /* 5*/ "EDGE_WEIGHT_TYPE",                  // Distance calculation method
            /* 6*/ "EDGE_WEIGHT_FORMAT",                // Matrix storage format
            /* 7*/ "DISPLAY_DATA_TYPE",                 // Coordinate display type
            /* 8*/ "EDGE_WEIGHT_SECTION",               // Explicit distance matrix
            /* 9*/ "DISPLAY_DATA_SECTION",              // Display coordinates
            /*10*/ "NODE_COORD_SECTION",                // Node coordinates
            /*11*/ "NODE_COORD_TYPE",                   // Coordinate type
            /*12*/ "DEPOT_SECTION",                     // Depot node(s)
            /*13*/ "CAPACITY_VOL",                      // Volume capacity
            /*14*/ "DEMAND_SECTION",                    // Customer demands per day
            /*15*/ "TIME_WINDOW_SECTION",               // Time windows
            /*16*/ "STANDTIME_SECTION",                 // Service times
            /*17*/ "PICKUP_SECTION",                    // Pickup amounts
            /*18*/ "EOF",                               // End of file
            /*19*/ "NUMBER_OF_TRUCKS",                  // Fleet size
            /*20*/ "NUM_DAYS",                          // Planning periods (CTSP)
            /*21*/ "DISTANCE",                          // Max distance per day (CTSP)
            /*22*/ "MAXIMUM_ALLOWABLE_DIFFERENTIAL"};   // Max time differential (CTSP)

        /// Edge weight types (distance calculation methods)
        const vector<string> wtypes = {
            "EXPLICIT",     // Explicit distance matrix provided
            "EUC_2D",       // Euclidean distance in 2D
            "EUC_3D",       // Euclidean distance in 3D
            "MAX_2D",       // Maximum distance in 2D
            "MAX_3D",       // Maximum distance in 3D
            "MAN_2D",       // Manhattan distance in 2D
            "MAN_3D",       // Manhattan distance in 3D
            "CEIL_2D",      // Ceiling of Euclidean distance
            "GEO",          // Geographical distance (latitude/longitude)
            "ATT"};         // Special ATT distance function

        /// Edge weight matrix formats
        const vector<string> wformats = {
            "UPPER_ROW",        // Upper triangular, row-wise
            "LOWER_ROW",        // Lower triangular, row-wise
            "UPPER_DIAG_ROW",   // Upper triangular with diagonal, row-wise
            "LOWER_DIAG_ROW",   // Lower triangular with diagonal, row-wise
            "UPPER_COL",        // Upper triangular, column-wise
            "LOWER_COL",        // Lower triangular, column-wise
            "UPPER_DIAG_COL",   // Upper triangular with diagonal, column-wise
            "LOWER_DIAG_COL",   // Lower triangular with diagonal, column-wise
            "FULL_MATRIX"};     // Full distance matrix

        /// Display data types
        const vector<string> dtypes = {
            "COORD_DISPLAY",    // Display using coordinates
            "TWOD_DISPLAY",     // 2D display data
            "NO_DISPLAY"};      // No display data

        /// Diagnostic messages
        const vector<string> messages = {
            "--  Reading input file  --: \n"};

    private:
        // Instance metadata
        string name_;                           ///< Instance name
        string type_;                           ///< Problem type
        string comment_;                        ///< Comment string
        int dimension_;                         ///< Number of nodes (including depot)
        int edge_weight_type_;                  ///< Index of distance calculation method
        int edge_weight_format_;                ///< Index of matrix format
        int display_data_type_;                 ///< Index of display type
        int num_days_;                          ///< Number of planning days (CTSP)
        int max_distance_;                      ///< Maximum distance per day (CTSP)
        int maximum_allowable_differencial_;    ///< Maximum time differential (CTSP)
        int depot_;                             ///< Depot node index

        double *distances_;                     ///< Distance matrix (linear array)

        vector<int> coord_id_;                  ///< Node IDs for coordinates
        vector<coordType> coord_;               ///< Node coordinates

        vector<int> display_id_;                ///< Node IDs for display
        vector<coordType> display_;             ///< Display coordinates

        vector<vector<int>> demand_;            ///< Demands: demand_[node][day]

        /// Function pointers for distance calculations
        vector<distanceType> distance_function_;
        
        /// Function pointers for section reading
        vector<readType> read_function_;
        
        /// Function pointers for edge weight matrix reading
        vector<readType> edge_weight_reading_function_;

        /// Optimal/best-known values [no waiting, with waiting]
        vector<double> optimal_values_;

    public:
        /**
         * @brief Default constructor
         * Initializes all data structures and function pointer tables
         */
        TSPLIB_instance(void);
        
        /**
         * @brief Destructor
         * Frees dynamically allocated distance matrix
         */
        virtual ~TSPLIB_instance(void);

        /**
         * @brief Read and parse a TSPLIB-formatted file
         * @param input_file Path to the TSPLIB instance file
         * @param log_file Path to the log file for parsing diagnostics
         * 
         * This method:
         * 1. Preprocesses the file (removes colons)
         * 2. Parses all sections
         * 3. Computes distance matrix if needed
         * 4. Validates the instance
         */
        void read(const string &input_file, const string &log_file);

        // Getters for instance properties
        
        inline const string &get_instance_name(void) const
        {
            return name_;
        }

        inline const string &get_instance_type(void) const
        {
            return type_;
        }

        inline const string &get_instance_comment(void) const
        {
            return comment_;
        }

        inline const int &get_dimension(void) const
        {
            return dimension_;
        }

        inline const vector<double> &get_optimal_values(void) const
        {
            return optimal_values_;
        }

        inline const vector<coordType> &coord(void) const
        {
            return coord_;
        }

        /**
         * @brief Copy distance matrix to a GOMA matrix object
         * @param distances_ Output matrix (resized to dimension x dimension)
         * 
         * Converts internal linear distance array to 2D matrix format.
         * Sets diagonal elements to a large value (100000000.0).
         */
        void get_distances(GOMA::matrix<double> &distances_) const;

        inline const vector<vector<int>> &get_demands(void) const
        {
            return demand_;
        }

        inline const int &get_depot(void) const
        {
            return depot_;
        }

        inline const int &get_num_days(void) const
        {
            return num_days_;
        }

        inline const int &get_max_distance(void) const
        {
            return max_distance_;
        }

        inline const int &get_maximum_allowable_differencial(void) const
        {
            return maximum_allowable_differencial_;
        }

    private:
        /**
         * @brief Compute distance matrix from node coordinates
         * Uses the distance function specified by edge_weight_type_
         */
        void compute_implicit_distance_matrix_(void);

        // Section reading methods - parse specific sections of TSPLIB files
        void read_name_section_(istream &is, ostream &os);
        void read_type_section_(istream &is, ostream &os);
        void read_comment_section_(istream &is, ostream &os);
        void read_dimension_section_(istream &is, ostream &os);
        void read_capacity_section_(istream &is, ostream &os);
        void read_edge_weight_type_section_(istream &is, ostream &os);
        void read_edge_weigh_format_section_(istream &is, ostream &os);
        void read_edge_weight_section_(istream &is, ostream &os);
        void read_display_data_type_section_(istream &is, ostream &os);
        void read_node_coord_section_(istream &is, ostream &os);
        void read_node_coord_type_section_(istream &is, ostream &os);
        void read_depot_section_(istream &is, ostream &os);
        void read_capacity_vol_section_(istream &is, ostream &os);
        void read_demand_section_(istream &is, ostream &os);
        void read_time_window_section_(istream &is, ostream &os);
        void read_standtime_section_(istream &is, ostream &os);
        void read_pickup_section_(istream &is, ostream &os);
        void read_EOF_section_(istream &is, ostream &os);
        void read_number_of_trucks_section_(istream &is, ostream &os);
        void read_num_days_section_(istream &is, ostream &os);
        void read_distance_section_(istream &is, ostream &os);
        void read_display_data_section_(istream &is, ostream &os);
        void read_maximum_allowable_differencial_section_(istream &is, ostream &os);

        // Edge weight matrix reading methods for different formats
        void read_edge_weight_section_upper_row_(istream &is, ostream &os);
        void read_edge_weight_section_lower_row_(istream &is, ostream &os);
        void read_edge_weight_section_upper_diag_row_(istream &is, ostream &os);
        void read_edge_weight_section_lower_diag_row_(istream &is, ostream &os);
        void read_edge_weight_section_upper_col_(istream &is, ostream &os);
        void read_edge_weight_section_lower_col_(istream &is, ostream &os);
        void read_edge_weight_section_upper_diag_col_(istream &is, ostream &os);
        void read_edge_weight_section_lower_diag_col_(istream &is, ostream &os);
        void read_edge_weight_section_full_matrix_(istream &is, ostream &os);

        // Distance calculation methods for different metrics
        
        /**
         * @brief Euclidean distance in 2D space
         * @return Rounded Euclidean distance: round(sqrt((x1-x2)^2 + (y1-y2)^2))
         */
        double compute_euc_2d_distance_(const coordType &a, const coordType &b) const;
        
        /**
         * @brief Maximum coordinate difference (L-infinity norm)
         * @return max(|x1-x2|, |y1-y2|)
         */
        double compute_max_2d_distance_(const coordType &a, const coordType &b) const;
        
        /**
         * @brief Manhattan distance (L1 norm)
         * @return |x1-x2| + |y1-y2|
         */
        double compute_man_2d_distance_(const coordType &a, const coordType &b) const;
        
        /**
         * @brief Ceiling of Euclidean distance
         * @return ceil(sqrt((x1-x2)^2 + (y1-y2)^2))
         */
        double compute_ceil_2d_distance_(const coordType &a, const coordType &b) const;
        
        /**
         * @brief Geographical distance on Earth's surface
         * @return Distance in km using spherical law of cosines
         * 
         * Coordinates are interpreted as latitude/longitude in degrees.
         * Uses Earth radius of 6378.388 km.
         */
        double compute_geo_distance_(const coordType &a, const coordType &b) const;
        
        /**
         * @brief Special ATT distance function
         * @return Pseudo-Euclidean distance used in att48 and similar instances
         */
        double compute_att_distance_(const coordType &a, const coordType &b) const;

        /**
         * @brief Allocate memory for distance matrix and coordinate arrays
         * @param dimension Number of nodes
         */
        void establish_dimension_(const int dimension);

        /**
         * @brief Preprocess file by removing colons
         * @param i_file Input file path
         * @param o_file Output file path
         */
        void clean_(const string &i_file, const string &o_file);
        
        /**
         * @brief Preprocess stream by removing colons
         * @param is Input stream
         * @param os Output stream
         */
        void clean_(istream &is, ostream &os);

        /**
         * @brief Main parsing method - reads all sections
         * @param is Input stream
         * @param os Output stream (log)
         */
        void read_(istream &is, ostream &os);

        /**
         * @brief Find keyword index in keyword vector
         * @param token Keyword to search for
         * @param keywords Vector of valid keywords
         * @return Index of keyword, or -1 if not found
         */
        int find_key_(const string &token, const vector<string> keywords) const;

        /**
         * @brief Round to nearest integer
         * @param x Floating point value
         * @return Nearest integer
         */
        int nint_(double x) const;
        
        /**
         * @brief Truncate to integer (floor)
         * @param x Floating point value
         * @return Integer part
         */
        double dtrunc_(double x) const;
        
        /**
         * @brief Convert geographical coordinates to radians
         * @param a Coordinate in degrees (decimal format)
         * @param longitude Output longitude in radians
         * @param latitude Output latitude in radians
         */
        void radian_coords_(const coordType &a, double &longitude, double &latitude) const;
    };

}