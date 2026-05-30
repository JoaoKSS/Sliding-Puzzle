# Makefile para o projeto de 8-puzzle e 15-puzzle
# Algoritmos: A* e IDA*
#
# Referências:
#   Korf (1985) - IDA*
#   Hart, Nilsson & Raphael (1968) - A*

CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra
TARGET = puzzle

SRCS = main.cpp estado.cpp a_estrela.cpp ida_estrela.cpp
HDRS = estado.h a_estrela.h ida_estrela.h
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Executar todas as instâncias do 8-puzzle com ambos algoritmos
run_8puzzle: $(TARGET)
	./$(TARGET) 8puzzle_instances.txt 3 ambos

# Executar todas as instâncias do 15-puzzle com ambos algoritmos
run_15puzzle: $(TARGET)
	./$(TARGET) 15puzzle_instances.txt 4 ambos

# Executar tudo
run_all: run_8puzzle run_15puzzle

.PHONY: all clean run_8puzzle run_15puzzle run_all
