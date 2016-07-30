#include "catch.hpp"
#include "symcxx/core.hpp"

TEST_CASE( "is_prime", "[symcxx::PrimeSieve]" ) {
    auto ps = symcxx::PrimeSieve<>();
    REQUIRE( ps.is_prime(5) );
    REQUIRE( !ps.is_prime(6) );
    REQUIRE( ps.is_prime(7) );
}
