#!/bin/bash

run_program() {

    local algo=$1
    local dim=$2
    local SEED=$3
    perf stat -o rep.txt -e $metrics ./program "$algo" "$dim" $SEED 0 > /dev/null
    cat rep.txt

    # Extract the metrics from perf report
    time=$(cat rep.txt | grep "elapsed" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
    cycles=$(cat rep.txt | grep "cycles" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
    instructions=$(cat rep.txt | grep "instructions" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
    cache_misses=$(cat rep.txt | grep "cache-misses" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')
    cache_references=$(cat rep.txt | grep "cache-references" | awk -F ' ' '{print $1}' | tr -d ',' | tr -d ' ')

    # Calculate the Cycles per Instruction (CPI) and cache-miss rate
    cpi=0
    cache_miss_rate=0
    if [ "$instructions" -ne 0 ]; then
        cpi=$(echo "scale=10; $cycles / $instructions" | bc)
    fi
    if [ "$cache_references" -ne 0 ]; then
        cache_miss_rate=$(echo "scale=10; $cache_misses / $cache_references" | bc)
    fi

    # Print the results neatly
    echo "Performance Metrics:"
    echo "---------------------"
    echo "Elapsed Time       : $time seconds"
    echo "Cycles             : $cycles"
    echo "Instructions       : $instructions"
    echo "Cache Misses       : $cache_misses"
    echo "Cache References   : $cache_references"
    echo "CPI (Cycles/Instr) : $cpi"
    echo "Cache Miss Rate    : $cache_miss_rate"
}



# Data for perf to collect
metrics="cycles,instructions,cache-misses,cache-references"

SEED=42

# Run the program and collect metrics
./manfile > /dev/null
run_program "BLAS" "750" $SEED
