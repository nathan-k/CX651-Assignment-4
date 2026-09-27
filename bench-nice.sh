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

# Niceness rises evenly from 0 (default priority) on the first run to 19
# (lowest priority) on the last. Staying at 0 or above avoids needing root,
# since only root can raise a process's priority with negative niceness.
min_nice=0
max_nice=19

output_dir="data/bench-nice-${matrix_size}"

echo "$(date)"
echo "Starting ${num_processes} concurrent ${matrix_size}x${matrix_size} matrix multiplications with mixed priorities"

mkdir -p "$output_dir"

pids=()

for i in $(seq 1 "$num_processes")
do
    if (( num_processes == 1 )); then
        niceness=$min_nice
    else
        # Rounded linear interpolation between min_nice and max_nice
        span=$(( max_nice - min_nice ))
        steps=$(( num_processes - 1 ))
        niceness=$(( min_nice + ((i - 1) * span + steps / 2) / steps ))
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
