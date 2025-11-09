# CTSP Instance Reader

A C++ library for reading and parsing Consistent Traveling Salesman Problem (CTSP) instances in TSPLIB format.

## Overview

This software provides comprehensive support for reading, parsing, and validating CTSP instance files. It includes:

- **TSPLIB Format Parser**: Full support for the TSPLIB file format with CTSP extensions
- **Multiple Distance Metrics**: Euclidean, Manhattan, Geographical, and other distance calculations
- **Instance Validation**: Automatic checking of triangle inequality and symmetry properties
- **CTSP Extensions**: Support for multi-day planning, customer demands, and consistency constraints

## What is CTSP?

The Consistent Traveling Salesman Problem (CTSP) is a variant of the classical TSP where:
- Customers must be visited across multiple days/periods
- There is a maximum allowable time differential (T) for service times at each customer
- Each day has a maximum distance constraint
- Solutions must maintain consistency in service patterns across days

## Features

### Supported Instance Types
- **TSP**: Classical Traveling Salesman Problem
- **PTSP**: Periodic TSP with time-varying demands
- **CTSP**: Consistent TSP with multi-day planning

### Supported Distance Metrics
- `EUC_2D`: Euclidean distance in 2D
- `MAN_2D`: Manhattan distance
- `MAX_2D`: Maximum coordinate difference
- `CEIL_2D`: Ceiling of Euclidean distance
- `GEO`: Geographical distance (latitude/longitude)
- `ATT`: Pseudo-Euclidean distance (ATT instances)
- `EXPLICIT`: Explicit distance matrix

### Matrix Storage Formats
- Full matrix
- Upper/lower triangular (with or without diagonal)
- Row-wise or column-wise storage

## Building

The project uses CMake for building:

```bash
mkdir build
cd build
cmake ../source
make
```

This will create the `ctsp_reader` executable in `build/main/`.

## Usage

```bash
./ctsp_reader <input_file> <log_file>
```

### Arguments
- `input_file`: Path to the CTSP instance file (TSPLIB format)
- `log_file`: Path where parsing diagnostics will be written

### Example

```bash
./build/main/ctsp_reader input/SubramanyamGounaris/bayg29_p5_f50_lL.contsp output/bayg29.log
```

## Instance File Format

CTSP instances extend the TSPLIB format with additional fields:

```
NAME: instance_name
TYPE: CTSP
COMMENT: optimal_no_wait, optimal_with_wait
DIMENSION: n_nodes
EDGE_WEIGHT_TYPE: EUC_2D
NUM_DAYS: n_days
DISTANCE: max_distance_per_day
MAXIMUM_ALLOWABLE_DIFFERENTIAL: T
NODE_COORD_SECTION
...
DEMAND_SECTION
...
EOF
```

### Key Fields

- **NUM_DAYS**: Number of planning periods
- **DISTANCE**: Maximum distance allowed per day
- **MAXIMUM_ALLOWABLE_DIFFERENTIAL**: Consistency constraint (T parameter)
- **DEMAND_SECTION**: Customer demands per day

## Project Structure

```
CTSP_instance_reader/
├── source/
│   ├── IO/
│   │   ├── include/
│   │   │   ├── CTSP_instance.hpp      # CTSP instance class
│   │   │   ├── PTSP_instance.hpp      # PTSP base class
│   │   │   ├── TSPLIB_instance.hpp    # TSPLIB parser
│   │   │   └── matrix.hpp             # Matrix template class
│   │   └── src/
│   │       ├── CTSP_instance.cpp
│   │       ├── PTSP_instance.cpp
│   │       └── TSPLIB_instance.cpp
│   └── main/
│       └── src/
│           └── main.cpp               # Main entry point
├── input/                             # Example instances
│   └── SubramanyamGounaris/          # CTSP benchmark instances
└── build/                            # Build directory
```

## API Overview

### CTSP::instance Class

Main class for working with CTSP instances:

```cpp
#include "CTSP_instance.hpp"

// Read instance from file
CTSP::instance inst("instance.contsp", "log.txt");

// Access instance data
const auto& distances = inst.get_distances();
const auto& demands = inst.get_demands();
double max_dist = inst.get_max_distance();
const auto& T = inst.get_T();
size_t n_customers = inst.get_n_customers();
size_t n_days = inst.get_n_days();

// Check properties
bool is_symmetric = inst.symmetry();
bool satisfies_triangle = inst.triangle_inequality();
```

### Key Methods

- `get_distances()`: Returns the distance matrix
- `get_demands()`: Returns customer demands per day
- `get_max_distance()`: Returns maximum distance per day
- `get_T()`: Returns maximum time differential per customer
- `get_n_customers()`: Returns number of customers
- `get_n_days()`: Returns number of planning days
- `get_optimal_values()`: Returns best-known solution values

## Benchmark Instances

The `input/SubramanyamGounaris/` directory contains CTSP benchmark instances with naming convention:

```
<tsp_base>_p<n_days>_f<fill_percentage>_l<T_level>.contsp
```

Where:
- `tsp_base`: Base TSP instance (e.g., bayg29, att48)
- `n_days`: Number of planning days (3 or 5)
- `fill_percentage`: Demand density (50%, 70%, 90%)
- `T_level`: Consistency constraint level (L=Low, M=Medium, H=High)

## Dependencies

- C++11 or later
- CMake 3.10 or later
- Standard C++ library

## License

See [LICENSE](LICENSE) file for details.

## References

- TSPLIB: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/
- Consistent TSP: Subramanyam, A., & Gounaris, C. E. (2016). A branch-and-cut framework for the consistent traveling salesman problem. European Journal of Operational Research, 248(2), 384-395.
- Consistent TSP: Subramanyam, A., & Gounaris, C. E. (2018).  A Decomposition Algorithm for the Consistent Traveling Salesman Problem with Vehicle Idling. Transportation Science, 52(2), 386–401.

## Contributing

This is an open-source project. Contributions, bug reports, and suggestions are welcome!

## Authors

Repository maintained by RieraULL.
