CXX = g++
CXXFLAGS = -std=c++17 -O3
TARGET = nets_engine.exe

.PHONY: all game clean

all: $(TARGET)

$(TARGET): nets_engine.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

game: $(TARGET)
	cd netgame && mvn javafx:run

clean:
	rm -f $(TARGET)
	cd netgame && mvn clean