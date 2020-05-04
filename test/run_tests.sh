#!/bin/bash
# Run unit tests

set -e 

TEST_BIN_DIR="bin/test"

for unit_test in "$TEST_BIN_DIR"/*
do
    echo "Running $unit_test"
    if ! ./"$unit_test"; then
        echo "Test $unit_test failed"
        exit 1
    fi
done
