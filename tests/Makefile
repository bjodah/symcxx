CXX ?= g++
CXXFLAGS ?= -I../include
override CXXFLAGS += -std=c++11 -Wall -Wextra -Werror -Wpadded -pedantic -fno-omit-frame-pointer

.PHONY: test debug clean

test: catch_symcxx
	ulimit -Sv 64000; ./catch_symcxx

debug: debug_symcxx
	ulimit -Sv 512000; valgrind ./debug_symcxx

clean:
	rm *.o
	rm *_symcxx


basic.o: ../src/basic.cpp ../include/symcxx/*.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

namespace.o: ../src/namespace.cpp ../include/symcxx/*.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test_%.o: test_%.cpp ../include/symcxx/*.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^


TESTFILES=catch_symcxx.o test_isprime.o test_namespace.o test_basic.o basic.o namespace.o

catch_symcxx: $(TESTFILES) ../include/symcxx/core.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(TESTFILES) $(LDFLAGS)

debug_symcxx: debug_symcxx.cpp basic.o namespace.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

