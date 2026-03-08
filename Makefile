CXX = g++
CXXFLAGS = -std=c++17 -O3
TARGET = nets_engine.exe

.PHONY: all game clean build

all: $(TARGET)

$(TARGET): nets_engine.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

build: $(TARGET)
	cd netgame && mvn compile

game: build
	cd netgame && mvn javafx:run

clean:
	rm -f $(TARGET)
	cd netgame && mvn clean