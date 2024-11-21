#!/bin/bash
# Note: Make sure to run the manfile in the root directory with matrix_mult_benchmark.c
# to get the correct program when compiling using manfile.

sum_array() {
    local arr=("$@")
    local total=0
    for val in "${arr[@]}"; do
        total=$(echo "scale=10; $total + $val" | bc)
    done

    echo "$total"
}

# Assumes NUM_RUNS > 1.
calculate_sample_variance() {
    # First argument
    local mean=$1
    # Shift drops the first argument and shifts so that arg $n -> arg $n-1
    shift
    local NUM_RUNS=$1
    shift
    # Rest of arguments are the array elements
    local arr=("$@")
    local sse=0
    for val in "${arr[@]}"; do
        sse=$(echo "scale=10; $sse + ($mean - $val) * ($mean - $val)" | bc)
    done

    local_sample_variance=$(echo "scale=10; $sse / ($NUM_RUNS - 1)" | bc)

    echo "$local_sample_variance"
}

# Filename to store the benchmark data in
filename="benchmark/data/benchmark_results.csv"

# Add the headers / categories into the start of the CSV file.
# Use double quotes around $filename as safety practice to ensure
# interpretation as a single argument.
echo "Algorithm,Dimension,Average Execution Time (seconds),Cycles,Instructions,Cycles per Instruction (CPI),Cache-Misses,Cache-References,Cache-Miss-Rate,Execution Time Variance,Cycles Variance,Instructions Variance,CPI Variance,Cache-Misses Variance,Cache-References Variance,Cache-Miss-Rate Variance" > "$filename"

# Create array of algorithms to benchmark
algorithms=("BLAS" "NAIVE" "SINGLETHREAD" "MULTITHREAD" "MULTITHREAD_3AVX" "MULTITHREAD_9AVX")

# Create array of dimensions to benchmark
dimensions=(50 100 200 500 750 1000 1500 2000)

# Using previously found optimal block size (see run_block_size_benchmark.sh)
BLOCK_SIZE=128

# Number of runs for each (algorithm, dimension) benchmark
NUM_RUNS=40

# Seed for reproducability when running benchmark
SEED=42

# Data for perf to collect
metrics="cycles,instructions,cache-misses,cache-references"

# Run benchmark for each dimension and algorithm and append result in $filename
for algo in "${algorithms[@]}"; do

    # Newline
    echo ""

    # Compile and link the code to create benchmark program
    echo "Compiling and linking $algo..."
    ./manfile > /dev/null 2>&1

    for dimension in "${dimensions[@]}"; do

        # Perform warm-up
        echo "Warm-up $algo with dimension size of $dimension..."
        ./program $algo $dimension $SEED $BLOCK_SIZE 1 # 1 for using warm-up

        # Arrays to contain the data from each run
        record_time=()
        record_cycles=()
        record_instruction=()
        record_cpi=()
        record_cache_misses=()
        record_cache_references=()
        record_cache_miss_rate=()

        # Perform a single run and collect data using perf
        for (( run=0; run<NUM_RUNS; run++ )); do

            # Run perf and generate perf_report.txt file
            echo "Performing run $run..."
            perf stat -o perf_report.txt -e $metrics ./program $algo $dimension $SEED $BLOCK_SIZE 0 > /dev/null

            # Extract the metrics from perf report
            time=$(cat perf_report.txt | grep "elapsed" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
            cycles=$(cat perf_report.txt | grep "cycles" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
            instructions=$(cat perf_report.txt | grep "instructions" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
            cache_misses=$(cat perf_report.txt | grep "cache-misses" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
            cache_references=$(cat perf_report.txt | grep "cache-references" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')

            # Calculate the Cycles per Instruction (CPI) and cache-miss rate
            cpi=0
            cache_miss_rate=0
            if [ "$instructions" -ne 0 ]; then
                cpi=$(echo "scale=10; $cycles / $instructions" | bc)
            fi
            if [ "$cache_references" -ne 0 ]; then
                cache_miss_rate=$(echo "scale=10; $cache_misses / $cache_references" | bc)
            fi

            # Add each metric data to corresponding array
            record_time+=("$time")
            record_cycles+=("$cycles")
            record_instructions+=("$instructions")
            record_cache_misses+=("$cache_misses")
            record_cache_references+=("$cache_references")
            record_cpi+=("$cpi")
            record_cache_miss_rate+=("$cache_miss_rate")
        done

        # Calculate the total sum of each recorded data
        total_time=$(sum_array "${record_time[@]}")
        total_cycles=$(sum_array "${record_cycles[@]}")
        total_instructions=$(sum_array "${record_instructions[@]}")
        total_cpi=$(sum_array "${record_cpi[@]}")
        total_cache_misses=$(sum_array "${record_cache_misses[@]}")
        total_cache_references=$(sum_array "${record_cache_references[@]}")
        total_cache_miss_rate=$(sum_array "${record_cache_miss_rate[@]}")

        # Calculate the mean of each recorded data
        avg_time=$(echo "scale=10; $total_time / $NUM_RUNS" | bc)
        avg_cycles=$(echo "scale=10; $total_cycles / $NUM_RUNS" | bc)
        avg_instructions=$(echo "scale=10; $total_instructions / $NUM_RUNS" | bc)
        avg_cpi=$(echo "scale=10; $total_cpi / $NUM_RUNS" | bc)
        avg_cache_misses=$(echo "scale=10; $total_cache_misses / $NUM_RUNS" | bc)
        avg_cache_references=$(echo "scale=10; $total_cache_references / $NUM_RUNS" | bc)
        avg_cache_miss_rate=$(echo "scale=10; $total_cache_miss_rate / $NUM_RUNS" | bc)

        # Data to calculate the sample variance from benchmark runs
        variance_time=$(calculate_sample_variance "$avg_time" "$NUM_RUNS" "${record_time[@]}")
        variance_cycles=$(calculate_sample_variance "$avg_cycles" "$NUM_RUNS" "${record_cycles[@]}")
        variance_instructions=$(calculate_sample_variance "$avg_instructions" "$NUM_RUNS" "${record_instructions[@]}")
        variance_cpi=$(calculate_sample_variance "$avg_cpi" "$NUM_RUNS" "${record_cpi[@]}")
        variance_cache_misses=$(calculate_sample_variance "$avg_cache_misses" "$NUM_RUNS" "${record_cache_misses[@]}")
        variance_cache_references=$(calculate_sample_variance "$avg_cache_references" "$NUM_RUNS" "${record_cache_references[@]}")
        variance_cache_miss_rate=$(calculate_sample_variance "$avg_cache_miss_rate" "$NUM_RUNS" "${record_cache_miss_rate[@]}")

        # Write the result into the CSV file
        echo "$algo,$dimension,$avg_time,$avg_cycles,$avg_instructions,$avg_cpi,$avg_cache_misses,$avg_cache_references,$avg_cache_miss_rate,$variance_time,$variance_cycles,$variance_instructions,$variance_cpi,$variance_cache_misses,$variance_cache_references,$variance_cache_miss_rate" >> "$filename"
    done
done

# Clean-up
rm perf_report.txt

# Print the final result
cat "$filename"
