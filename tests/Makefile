CXX := g++
#CXX := clang++-3.7
CXXFLAGS ?= -std=c++11 -g -Wall -Wextra -Werror -Wpadded -pedantic -I../include -fno-omit-frame-pointer -O0 -ggdb
#-D_GLIBCXX_DEBUG
#-fsanitize=address 

.PHONY: test debug clean

test: catch_sym
	./catch_sym

debug: debug_sym
	valgrind ./debug_sym

clean:
	rm *.o
	rm *_sym


sym.o: ../src/sym.cpp ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test_%.o: test_%.cpp ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^


TESTFILES=catch_sym.o test_namespace.o test_basic.o sym.o

catch_sym: $(TESTFILES)
	$(CXX) $(CXXFLAGS) -o $@ $(TESTFILES)

debug_sym: debug_sym.cpp sym.o
	$(CXX) $(CXXFLAGS) -o $@ $^

