CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

SRC = main.cpp \
      src/splay/SplayTree.cpp \
      src/avl/AVLTree.cpp \
      src/experiments/Experiments.cpp

TARGET = tarea2

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o