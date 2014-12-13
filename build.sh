#!/bin/bash
# Call this to build the program

# The output folder
OUTPUT="build/"

# Create the folder
[ -d "$OUTPUT" ] || mkdir -p $OUTPUT

# Change to the folder
cd $OUTPUT

# Create all dependencies
cmake ../src

# Start the making process
make
