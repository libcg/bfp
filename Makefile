CC = gcc
CXX = g++
FLAGS = -Ilib -Itest -O2 -Wall -g
CFLAGS = -std=c99 $(FLAGS)
CXXFLAGS = -std=c++11 $(FLAGS)

LIB_TARGET = lib/libbfp.a
LIB_OBJ = lib/posit.o lib/pack.o lib/util.o lib/op1.o lib/op2.o

TEST_TARGET = test/bfptest
TEST_OBJ = test/test.o test/p2_test.o test/p3_test.o test/ieee_test.o \
           test/CuTest.o $(LIB_TARGET)

TARGET = bfp
OBJ = main.o $(LIB_TARGET)

all: $(TARGET) $(TEST_TARGET)

test: $(TEST_TARGET)
	./test/bfptest

clean:
	rm -f lib/*.o $(LIB_TARGET)
	rm -f test/*.o $(TEST_TARGET)
	rm -f *.o $(TARGET)

$(LIB_TARGET): $(LIB_OBJ)
	ar rcs $@ $^

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) -o $@ $^

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $^
