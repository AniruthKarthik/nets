.PHONY: game build test clean

# Default target: Run the Java game
game: build
	chmod +x netgame/mvnw
	cd netgame && ./mvnw javafx:run

# Build the C++ engine
build:
	g++ -std=c++17 nets_engine.cpp -o nets_engine

# Run C++ tests
test:
	g++ -std=c++17 testing/tester.cpp -o testing/tester
	./testing/tester

# Clean up build artifacts
clean:
	rm -f nets_engine testing/tester
	chmod +x netgame/mvnw
	cd netgame && ./mvnw clean
