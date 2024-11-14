#!/bin/bash

# Note: Make sure to run the manfile in the root directory with matrix_mult_benchmark.c.

# Filename to store the benchmark data in
filename="benchmark/benchmark_results.csv"
time_filename="benchmark_time.txt"

# Add the headers / categories into the start of the CSV file.
# Use double quotes around $filename as safety practice to ensure
# interpretation as a single argument.
echo "Algorithm,Dimension,Average Execution Time (seconds),Cycles,Instructions,Cycles per Instruction (CPI),Cache-Misses,Cache-References,Cache-Miss-Rate" > "$filename"
# Create file to have it ready for benchmark program
touch benchmark_time.txt

# Create array of algorithms to benchmark
algorithms=("BLAS" "NAIVE" "SINGLETHREAD" "MULTITHREAD" "MULTITHREAD_9AVX")

# Create array of dimensions to benchmark
dimensions=(50 100 1000 2000 3000)

# Benchmark program parameters
seed=27

# Run benchmark for each dimension and algorithm and append result in $filename
for dimension in "${dimensions[@]}"; do
    for algo in "${algorithms[@]}"; do

        # Newline
        echo ""

        # Compile and link the code to create benchmark program
        echo "Compiling and linking $algo..."
        ./manfile > /dev/null 2>&1

        # Run the benchmark program
        echo "Benchmarking $algo with dimension size of $dimension..."
        ./program $algo $dimension $seed 1 # 1 for using warm-up

        # Retrieve average execution time
        avg_time=0
        if [ -f "$time_filename" ]; then
            avg_time=$(cat "$time_filename")
        else
            echo "Error: No such file exists"
            exit 1
        fi

        # Use perf tool to retrieve performance metrics
        metrics="cycles,instructions,cache-misses,cache-references"

        # Run perf and generate perf_report.txt file
        echo "Generate perf report and metrics..."
        perf stat -o perf_report.txt -e $metrics ./program $algo $dimension $seed 0 > /dev/null

        # Extract the metrics
        cycles=$(cat perf_report.txt | grep "cycles" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
        instructions=$(cat perf_report.txt | grep "instructions" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
        cache_misses=$(cat perf_report.txt | grep "cache-misses" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
        cache_references=$(cat perf_report.txt | grep "cache-references" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')

        # Calculate the Cycles per Instruction (CPI) and cache-miss rate
        cpi="N/A"
        cache_miss_rate="N/A"
        if [ "$instructions" -ne 0 ]; then
            cpi=$(echo "scale=5; $cycles / $instructions" | bc)
        fi
        if [ "$cache_references" -ne 0 ]; then
            cache_miss_rate=$(echo "scale=5; $cache_misses / $cache_references" | bc)
        fi
        # Write the result into the CSV file
        echo "$algo,$dimension,$avg_time,$cycles,$instructions,$cpi,$cache_misses,$cache_references,$cache_miss_rate" >> "$filename"
    done
done

# Clean-up
rm $time_filename
rm perf_report.txt

# Print the final result
cat "$filename"
