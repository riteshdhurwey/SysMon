CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC = src/main.cpp \
      src/cpu.cpp \
      src/ResourceMonitor.cpp \
      src/process.cpp \
      src/systemMonitor.cpp \
      src/display.cpp 

OBJ = $(SRC:.cpp=.o)

TARGET = sysmon

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)