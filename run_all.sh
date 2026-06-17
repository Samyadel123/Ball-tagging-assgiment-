#!/bin/bash

# Exit on error
set -e

run_solution() {
    SOL_DIR=$1
    echo "=========================================================="
    echo "  RUNNING CROSS-LANGUAGE SOLUTION: $SOL_DIR"
    echo "=========================================================="

    # Build Java
    echo "[BUILD] Compiling Java (Player A)..."
    cd $SOL_DIR/java
    javac -cp .:json.jar PlayerA.java
    cd ../..

    # Build C++
    echo "[BUILD] Compiling C++ (Player B)..."
    g++ $SOL_DIR/cpp/PlayerB.cpp -o $SOL_DIR/cpp/player_b

    # Start Host (Background)
    echo "[RUN] Starting Python Host (Code C)..."
    python3 $SOL_DIR/python/host.py &
    HOST_PID=$!

    # Wait for host to start
    sleep 2

    # Start Java Player (Background)
    echo "[RUN] Starting Java Player (Code A)..."
    java -cp $SOL_DIR/java:$SOL_DIR/java/json.jar PlayerA &
    JAVA_PID=$!

    # Start C++ Player (Background)
    echo "[RUN] Starting C++ Player (Code B)..."
    ./$SOL_DIR/cpp/player_b &
    CPP_PID=$!

    # Wait for Host to finish (it handles the game timer)
    wait $HOST_PID || true

    # Clean up any remaining processes
    kill $JAVA_PID $CPP_PID 2>/dev/null || true
    echo -e "\n[DONE] $SOL_DIR completed.\n"
}

# Run all 3 solutions
run_solution "solution1_mutex"
run_solution "solution2_atomic"
run_solution "solution3_message"
