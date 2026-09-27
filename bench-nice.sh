#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <number-of-processes> <matrix-size>"
    exit 1
fi

num_processes=$1
matrix_size=$2

if ! [[ "$num_processes" =~ ^[1-9][0-9]*$ ]] ||
   ! [[ "$matrix_size" =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: both arguments must be positive integers."
    exit 1
fi

# Odd-numbered runs are high priority, even-numbered runs are low priority.
# Negative niceness needs root, so the default priority (0) is the high end.
high_nice=0
low_nice=19

output_dir="data/bench-nice-${matrix_size}"

echo "$(date)"
echo "Starting ${num_processes} concurrent ${matrix_size}x${matrix_size} matrix multiplications with mixed priorities"

mkdir -p "$output_dir"

pids=()

for i in $(seq 1 "$num_processes")
do
    if (( i % 2 == 1 )); then
        niceness=$high_nice
    else
        niceness=$low_nice
    fi

    /usr/bin/time \
        -f "CPU: %P" \
        -o "${output_dir}/mm-${i}-cpu.out" \
        nice -n "$niceness" ./bench "$matrix_size" "$matrix_size" "$matrix_size" 0 \
        > "${output_dir}/mm-${i}.out" &

    pids+=($!)

    echo "Started process ${i} (nice ${niceness})"
done

echo "Waiting for matrix multiplications"

for pid in "${pids[@]}"
do
    wait "$pid"
done

echo "Benchmark complete"
echo "$(date)"
