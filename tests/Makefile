#CXX := g++
CXX := clang++-3.7
CXXFLAGS ?= -std=c++11 -g -Wall -I../include #-D_GLIBCXX_DEBUG
# -ggdb

.PHONY: all

all: catch_sym
	./catch_sym

sym.o: ../src/sym.cpp ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test_%.o: test_%.cpp ../include/sym.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^


TESTFILES=catch_sym.o test_namespace.o test_basic.o sym.o

catch_sym: $(TESTFILES)
	$(CXX) $(CXXFLAGS) -o $@ $(TESTFILES)
