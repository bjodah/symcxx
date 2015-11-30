CXX := g++
#CXX := clang++-3.7
CXXFLAGS ?= -std=c++11 -g -Wall -Wextra -Werror -Wpadded -pedantic -I../include -fno-omit-frame-pointer -O0 -ggdb
#-D_GLIBCXX_DEBUG
#-fsanitize=address 

.PHONY: test debug clean

test: catch_symcxx
	./catch_symcxx

debug: debug_symcxx
	valgrind ./debug_symcxx

clean:
	rm *.o
	rm *_symcxx


symcxx.o: ../src/symcxx.cpp ../include/symcxx/*.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test_%.o: test_%.cpp ../include/symcxx/*.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^


TESTFILES=catch_symcxx.o test_namespace.o test_basic.o symcxx.o

catch_symcxx: $(TESTFILES)
	$(CXX) $(CXXFLAGS) -o $@ $(TESTFILES)

debug_symcxx: debug_symcxx.cpp symcxx.o
	$(CXX) $(CXXFLAGS) -o $@ $^

