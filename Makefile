CXX = g++
CXXFLAGS = -Ilib -O2 -Wall -g

LIB_TARGET = lib/libbfp.a
LIB_OBJ = lib/posit.o

TARGET = bfp
OBJ = main.o $(LIB_TARGET)

all: $(TARGET)

clean:
	rm -f *.o $(TARGET) lib/*.o lib/$(LIB_TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

$(LIB_TARGET): $(LIB_OBJ)
	ar rcs $@ $^

%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $^

