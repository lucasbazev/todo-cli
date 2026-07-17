CXXFLAGS += -Isrc

app: src/main.cpp src/domain/*.cpp src/storage/*.cpp src/cli/*.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ -lsqlite3

run: app
	./app

.PHONY: run app
