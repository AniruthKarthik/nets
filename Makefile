CXX = g++
CXXFLAGS = -std=c++17 -O3
TARGET = nets_engine.exe

# OS and Shell Detection
ifeq ($(OS),Windows_NT)
    # Windows Detect (CMD vs Sh)
    # In CMD, echo "check" returns "check". In Sh, it returns check.
    ifeq ($(shell echo "check"),"check")
        # Standard Windows CMD
        RM = del /f /q
        MVN = mvn.cmd
        SEP = &
    else
        # Windows with a Unix-like shell (Git Bash, MSYS2, etc.)
        RM = rm -f
        MVN = mvn
        SEP = &&
    endif
else
    # Linux / Mac
    RM = rm -f
    MVN = mvn
    SEP = &&
endif

.PHONY: all build clean game

all: $(TARGET)

$(TARGET): nets_engine.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

build: $(TARGET)
	cd netgame $(SEP) $(MVN) compile

game: build
	cd netgame $(SEP) $(MVN) javafx:run

clean:
	$(RM) $(TARGET)
	cd netgame $(SEP) $(MVN) clean
