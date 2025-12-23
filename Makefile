# Makefile for the Nets project

# Variables
CPP_ENGINE = Nets/nets_engine
CPP_SOURCE = Nets/nets_engine.cpp
JAVA_DIR = Nets/netgame

# Default target
all: run

# Run the full application
run: build_engine run_java

# Build the C++ engine
build_engine:
	@echo "Building C++ engine..."
	g++ -std=c++17 $(CPP_SOURCE) -o $(CPP_ENGINE)

# Run the JavaFX application
run_java:
	@echo "Running JavaFX application..."
	cd $(JAVA_DIR) && mvn javafx:run

# Clean the project
clean:
	@echo "Cleaning up..."
	rm -f $(CPP_ENGINE)

.PHONY: all run build_engine run_java clean
