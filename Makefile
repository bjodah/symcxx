CXX := g++
#clang++-3.7
CXXFLAGS ?= -std=c++11 -g
# -ggdb

.PHONY: all

all: test_sym
	./test_sym

test_sym: test_sym.cpp sym.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
