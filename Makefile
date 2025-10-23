CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

SRC = src/main.cpp src/database.cpp src/frecent.cpp src/search.cpp src/cli.cpp src/utils.cpp
TARGET = zpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o src/*.o

