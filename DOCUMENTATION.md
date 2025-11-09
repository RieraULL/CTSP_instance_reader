# CTSP Instance Reader - Code Documentation

## Overview

This document provides detailed information about the codebase structure and implementation of the CTSP Instance Reader library.

## Architecture

The project follows a layered architecture:

```
┌─────────────────────────────┐
│      Main Application       │
│       (main.cpp)           │
└─────────────────────────────┘
              │
              ▼
┌─────────────────────────────┐
│    CTSP::instance Class     │
│   (CTSP_instance.hpp/cpp)   │
└─────────────────────────────┘
              │
              ▼
┌─────────────────────────────┐
│    PTSP::instance Class     │
│   (PTSP_instance.hpp/cpp)   │
└─────────────────────────────┘
              │
              ▼
┌─────────────────────────────┐
│  TSP::TSPLIB_instance Class │
│ (TSPLIB_instance.hpp/cpp)   │
└─────────────────────────────┘
              │
              ▼
┌─────────────────────────────┐
│   GOMA::matrix<T> Class     │
│      (matrix.hpp)           │
└─────────────────────────────┘
```

## Core Classes

### 1. CTSP::instance

**Purpose**: Represents a complete Consistent TSP instance with all constraints and parameters.

**Key Responsibilities**:
- Read CTSP instances from TSPLIB-formatted files
- Store consistency constraints (maximum time differential T)
- Store maximum distance per day
- Provide access to optimal/best-known values
- Count customer operations across all days

**Important Members**:
```cpp
vector<double> T_;                    // Max time differential per customer
double max_distance_;                 // Max distance per day
vector<double> optimal_values_;       // [no_wait, with_wait]
```

**Usage Example**:
```cpp
CTSP::instance inst("instance.contsp", "log.txt");
double max_dist = inst.get_max_distance();
size_t operations = inst.get_n_customer_operations();
```

---

### 2. PTSP::instance

**Purpose**: Base class for Periodic/time-varying TSP instances.

**Key Responsibilities**:
- Store basic instance information (name, type, comment)
- Manage customer demands across multiple days
- Store distance matrix
- Validate triangle inequality and symmetry properties

**Important Members**:
```cpp
GOMA::matrix<double> distances_;      // Distance matrix
vector<vector<int>> demands_;         // demands_[customer][day]
bool triangle_inequality_;            // Validation flag
bool symmetry_;                       // Validation flag
```

**Validation Methods**:
- `check_triangle_inequality_()`: Verifies d(i,j) ≤ d(i,k) + d(k,j)
- `check_symmetry_()`: Verifies d(i,j) == d(j,i)

---

### 3. TSP::TSPLIB_instance

**Purpose**: Comprehensive parser for TSPLIB file format.

**Key Responsibilities**:
- Parse TSPLIB file format (with extensions)
- Compute distance matrices from coordinates
- Support multiple distance metrics
- Handle various matrix storage formats

**Distance Metrics Supported**:
- **EUC_2D**: Rounded Euclidean distance
- **MAN_2D**: Manhattan (L1) distance
- **MAX_2D**: L-infinity distance
- **CEIL_2D**: Ceiling of Euclidean distance
- **GEO**: Geographical distance (spherical)
- **ATT**: Pseudo-Euclidean (for specific instances)
- **EXPLICIT**: Pre-computed distance matrix

**Matrix Formats Supported**:
- Full matrix
- Upper/Lower triangular (with or without diagonal)
- Row-wise or column-wise storage

**Function Pointer Tables**:
```cpp
vector<distanceType> distance_function_;           // Distance calculators
vector<readType> read_function_;                   // Section parsers
vector<readType> edge_weight_reading_function_;   // Matrix readers
```

---

### 4. GOMA::matrix<T>

**Purpose**: Generic 2D matrix template with mathematical (1-based) indexing.

**Key Features**:
- 1-based indexing: `matrix(i, j)` where i,j ∈ [1, dimension]
- Dynamic resizing with data preservation
- Efficient linear storage
- Stream I/O operators

**Usage Example**:
```cpp
GOMA::matrix<double> dist(10, 10);  // 10x10 matrix
dist(1, 2) = 5.0;                   // Set element at row 1, col 2
double val = dist(1, 2);            // Read element
```

**Important Methods**:
- `resize(m, n)`: Resize (discards data)
- `resize_and_keep(m, n)`: Resize preserving overlap
- `fill(value)`: Set all elements
- `transpose(M)`: Compute transpose

---

## File Format

### CTSP Instance Format

CTSP instances extend TSPLIB with additional keywords:

```
NAME: <instance_name>
TYPE: CTSP
COMMENT: <optimal_no_wait>, <optimal_with_wait>
DIMENSION: <n_nodes>
EDGE_WEIGHT_TYPE: <distance_type>
NUM_DAYS: <n_days>
DISTANCE: <max_distance_per_day>
MAXIMUM_ALLOWABLE_DIFFERENTIAL: <T_value>
NODE_COORD_SECTION
<node_id> <x> <y>
...
DEMAND_SECTION
<node_id> <demand_day1> <demand_day2> ... <demand_dayN>
...
EOF
```

### Parsing Process

1. **Preprocessing**: Remove colons from keywords
2. **Keyword Recognition**: Match tokens to keyword table
3. **Section Dispatch**: Call appropriate parser using function pointers
4. **Distance Computation**: Calculate or read distance matrix
5. **Validation**: Check triangle inequality and symmetry

---

## Distance Calculation Details

### Euclidean Distance (EUC_2D)
```cpp
double dist = round(sqrt((x1-x2)^2 + (y1-y2)^2))
```

### Manhattan Distance (MAN_2D)
```cpp
double dist = round(|x1-x2| + |y1-y2|)
```

### Geographical Distance (GEO)
```cpp
// Convert degrees.minutes to radians
// Use spherical law of cosines with R = 6378.388 km
double dist = R * acos(0.5 * ((1+cos(Δlong))*cos(Δlat) - (1-cos(Δlong))*cos(lat_sum)))
```

### ATT Distance
```cpp
// Pseudo-Euclidean for ATT instances
double r = sqrt((dx^2 + dy^2) / 10.0)
double t = round(r)
double dist = (t < r) ? t + 1 : t
```

---

## Memory Management

### Distance Matrix Storage

The TSPLIB parser uses linear array storage:
```cpp
double* distances_ = new double[dimension * dimension];
// Access: distances_[i * dimension + j]
```

This is later converted to GOMA::matrix for easier access:
```cpp
GOMA::matrix<double> distances(dimension, dimension);
// Access: distances(i+1, j+1)  // 1-based
```

### Resource Cleanup

All classes follow RAII principles:
- Constructors allocate resources
- Destructors clean up automatically
- No manual memory management needed by users

---

## Validation

### Triangle Inequality Check

For all distinct nodes i, j, k:
```cpp
if (dist(i,k) + dist(k,j) < dist(i,j) - tolerance) {
    // Violation detected
}
```

Tolerance accounts for rounding in distance calculations.

### Symmetry Check

For all pairs i, j:
```cpp
if (|dist(i,j) - dist(j,i)| > tolerance) {
    // Asymmetry detected
}
```

---

## Error Handling

The library uses several error handling mechanisms:

1. **Assertions**: For programming errors (invalid indices)
2. **cerr Messages**: For data validation warnings
3. **exit(1)**: For critical parsing errors

Example warnings:
- "Warning: Triangle inequality violated"
- "Warning: Distances are not symmetric"

Example critical errors:
- "ERROR opening input file"
- "Edge weight type not defined"

---

## Extension Points

### Adding New Distance Metrics

1. Add enum to distance type definitions
2. Implement distance calculation method
3. Register in distance_function_ table

### Adding New TSPLIB Keywords

1. Add keyword to keywords vector
2. Implement read_*_section_() method
3. Register in read_function_ table

### Adding New Matrix Formats

1. Add format to wformats vector
2. Implement read_edge_weight_section_*_() method
3. Register in edge_weight_reading_function_ table

---

## Performance Considerations

### Time Complexity

- **Reading instance**: O(n²) for distance matrix
- **Triangle inequality check**: O(n³)
- **Symmetry check**: O(n²)

Where n is the number of nodes.

### Space Complexity

- **Distance matrix**: O(n²)
- **Demands**: O(n × d) where d is number of days
- **Coordinates**: O(n)

### Optimization Opportunities

1. Sparse matrix storage for large instances
2. Parallel validation checks
3. Lazy distance computation
4. Memory-mapped file I/O for very large instances

---

## Testing

### Instance Validation

The program automatically validates:
- File format correctness
- Triangle inequality property
- Symmetry property

### Sample Test

```bash
./ctsp_reader input/SubramanyamGounaris/bayg29_p5_f50_lL.contsp output/test.log
```

Check output log for:
- Dimension: 30 (29 customers + 1 depot)
- Number of days: 5
- Distance metric used
- Validation results

---

## Future Enhancements

Potential improvements:

1. **Exception Handling**: Replace exit() with exceptions
2. **Incremental Parsing**: Support streaming large files
3. **Output Formats**: Support writing instances in various formats
4. **Instance Generator**: Tools to create synthetic instances
5. **Visualization**: Generate plots of instances and solutions
6. **Solution Validator**: Verify solution feasibility

---

## References

### TSPLIB Specification
- TSPLIB95 format documentation
- Available at: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/

### CTSP Papers
- Subramanyam, A., & Gounaris, C. E. (2016). "A branch-and-cut framework for the consistent traveling salesman problem." European Journal of Operational Research, 248(2), 384-395.
- Subramanyam, A., & Gounaris, C. E. (2018).  "A Decomposition Algorithm for the Consistent Traveling Salesman Problem with Vehicle Idling". Transportation Science, 52(2), 386–401.

### Implementation Notes
- Uses C++11 features (uniform initialization, auto, etc.)
- Template-based matrix implementation
- Function pointer tables for extensibility

---

## Contact and Support

For questions, bug reports, or contributions:
- GitHub: https://github.com/RieraULL/CTSP_instance_reader
- Issues: Use the GitHub issue tracker

---

*Last updated: November 2025*
