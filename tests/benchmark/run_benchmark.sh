#!/bin/bash

# Note: Make sure to run the manfile in the root directory with matrix_mult_benchmark.c.

# Filename to store the benchmark data in
filename="benchmark_results.csv"
time_filename="benchmark_time.txt"

# Add the headers / categories into the start of the CSV file.
# Use double quotes around $filename as safety practice to ensure
# interpretation as a single argument.
echo "Algorithm,Average Execution Time (seconds),Cycles per Instruction (CPI),Cache Misses" > "$filename"

# Create array of algorithms to benchmark
algorithms=("BLAS" "NAIVE" "SINGLETHREAD" "MULTITHREAD" "MULTITHREAD_9AVX")

#  Benchmark program parameters
dimension_size=3000
seed=42

# Run benchmark for each algorithm and append result in $filename
for algo in "${algorithms[@]}"; do
    echo "Benchmarking $algo..."

    # Compile and link the code to create benchmark program
    ./manfile

    # Run the benchmark program
    ./test $algo $dimension_size $seed

    # Retrieve average execution time
    avg_time=0
    if [ -f "$time_filename" ]; then
        avg_time=$(cat "$time_filename")
    else
        echo "Error: No such file exists"
        exit 1
    fi

    # Use perf tool to retrieve performance metrics
    metrics="cycles,instructions,cache-misses"

    # Run perf and generate perf-report.txt file
    perf stat -o perf_report.txt -e $metrics ./test $algo $dimension_size $seed

done

# For algo in algorithms
#
# Perform benchmark with the given algo and store time in a file
# Retrieve algorithm time and place it in the CSV file
#
# Use perf to retrieve report file
# Extract each of the perf data points and place them in the CSV file
#
# Add a new line to the CSV file so that it is ready for next algo


