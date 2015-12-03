CXX := g++
#CXX := clang++-3.7
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -Wpadded -pedantic -I../include -fno-omit-frame-pointer $(DEFINES)
ifeq ($(RELEASE),1)
  CXXFLAGS+=-DNDEBUG -O2
else
  CXXFLAGS+=-g -O0
# -ggdb
endif

.PHONY: test debug clean

test: catch_symcxx
	./catch_symcxx

debug: debug_symcxx
	valgrind ./debug_symcxx

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


TESTFILES=catch_symcxx.o test_namespace.o test_basic.o basic.o namespace.o

catch_symcxx: $(TESTFILES)
	$(CXX) $(CXXFLAGS) -o $@ $(TESTFILES)

debug_symcxx: debug_symcxx.cpp basic.o namespace.o
	$(CXX) $(CXXFLAGS) -o $@ $^

