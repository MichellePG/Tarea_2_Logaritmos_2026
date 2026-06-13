CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

SRC = main.cpp \
      src/splay/SplayTree.cpp \
      src/avl/AVLTree.cpp \
      src/experiments/Experiments.cpp

TARGET = tarea2

# Exponente de N para los experimentos de teoremas (sobreescribible: make run EXP=20)
EXP ?= 25

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Ejecuta toda la batería de experimentos.
run: $(TARGET)
	./$(TARGET) all $(EXP)

# Genera los gráficos a partir de los CSV en outputs/results/.
# Usa uv (gestiona las dependencias de Python automáticamente).
plots:
	uv run scripts/plot.py

clean:
	rm -f $(TARGET) *.o

.PHONY: all run plots clean
