app: todo.cpp
	clang++ todo.cpp -o todo_executable -lsqlite3 -w; \
	if [ $$? -ne 0 ]; then \
		echo "clang++ failed, trying g++..."; \
		g++ todo.cpp -o todo_executable -lsqlite3 -w; \
		if [ $$? -ne 0 ]; then \
			echo "Both clang++ and g++ failed to compile todo.cpp"; \
			exit 1; \
		fi; \
	fi

run:
	./todo_executable
