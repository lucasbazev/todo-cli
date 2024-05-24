#!/bin/bash

# Try to compile with clang++
clang++ todo.cpp -o todo_executable -lsqlite3 -w

# If clang++ fails, try to compile with g++
if [ $? -ne 0 ]; then
	echo "clang++ failed, trying g++..."
	g++ todo.cpp -o todo_executable -lsqlite3 -w

	# Check if g++ succeeded
	if [ $? -ne 0 ]; then
		echo "Both clang++ and g++ failed to compile todo.cpp"
		exit 1
	fi
fi

# Run the executable if compilation succeeded
./todo_executable
