CXX := g++
#clang++-3.7
CXXFLAGS ?= -std=c++11 -g -Wall -D_GLIBCXX_DEBUG -I../include
# -ggdb

.PHONY: all

all: catch_sym
	./catch_sym

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

test_%.o: test_%.cpp ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

sym.o: ../src/sym.cpp ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

TESTFILES=catch_sym.o test_namespace.o test_basic.o sym.o

catch_sym: $(TESTFILES) ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(TESTFILES)
