//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "symcxx/core.hpp"

int main( int argc, char* const argv[] )
{
  std::cout << symcxx::all_kind_names();
  int result = Catch::Session().run( argc, argv );
  return result;
}
