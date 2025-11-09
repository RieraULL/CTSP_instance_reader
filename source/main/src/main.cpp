/**
 * @file main.cpp
 * @brief Main entry point for CTSP instance reader
 * 
 * This program reads and validates Consistent Traveling Salesman Problem (CTSP)
 * instances from TSPLIB-formatted files.
 * 
 * Usage:
 *   ctsp_reader <input_file> <log_file>
 * 
 * Arguments:
 *   input_file - Path to the CTSP instance file (TSPLIB format with CTSP extensions)
 *   log_file   - Path to the output log file for parsing diagnostics
 * 
 * Example:
 *   ./ctsp_reader input/SubramanyamGounaris/bayg29_p5_f50_lL.contsp output/bayg29.log
 * 
 * The program will:
 * 1. Parse the instance file
 * 2. Extract all problem parameters (distances, demands, constraints)
 * 3. Validate triangle inequality and symmetry
 * 4. Write parsing diagnostics to the log file
 * 
 * Exit codes:
 *   0 - Success
 *   Non-zero - Error during parsing or file I/O
 */

#include <cstdio>
#include <cstdlib>

#include <string>

#include "CTSP_instance.hpp"


using namespace std;

/**
 * @brief Main function
 * @param argc Argument count (should be 3)
 * @param argv Argument vector: [program_name, input_file, log_file]
 * @return 0 on success
 */
int main(int argc, char **argv)
{
    // Parse command line arguments
    const string input_file{argv[1]};
    const string output_file{argv[2]};

    // Read and parse the CTSP instance
    // This will automatically validate and log the instance details
    CTSP::instance I(input_file, output_file);

    return 0;
}
