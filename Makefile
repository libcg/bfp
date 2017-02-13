CXX = g++
CXXFLAGS = -O2 -Wall -g
TARGET = bfp
OBJ = main.o posit.o

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $^

%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $^

