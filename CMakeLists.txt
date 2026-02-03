cmake_minimum_required(VERSION 3.20)
project(csvanalyzer)

# Commands 
# cmake -G "MinGW Makefiles" ..
# cmake --build .

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ---------------------------------------------
# Main executable
# ---------------------------------------------
add_executable(csvanalyzer
    src/main.cpp
    src/csv_analyzer.cpp
    src/table.cpp
)

