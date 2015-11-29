#include "catch.hpp"
#include "sym.hpp"

TEST_CASE( "homogenous ordering", "[sym::Symbol]" ) {
    auto s0 = sym::Symbol(0);
    auto s1 = sym::Symbol(1);

    REQUIRE( s0 < s1 );
    REQUIRE( not (s1 < s0) );

    REQUIRE( s0 <= s1 );
    REQUIRE( not (s1 <= s0) );
    REQUIRE( s0 <= s0 );

    REQUIRE( s0 == s0 );
    REQUIRE( not (s1 == s0) );

    REQUIRE( s0 != s1 );
    REQUIRE( not (s1 != s1) );

    REQUIRE( s1 >= s0 );
    REQUIRE( not (s0 >= s1) );
    REQUIRE( s1 >= s1 );

    REQUIRE( s1 > s0 );
    REQUIRE( not (s0 > s1) );
}

TEST_CASE( "heterogeneous ordering", "[sym::Basic]" ) {
    auto s = sym::Symbol(1);
    auto i = sym::Integer(0);

    REQUIRE( s < i );
    REQUIRE( not (i < s) );

    REQUIRE( s <= i );
    REQUIRE( not (i <= s) );
    REQUIRE( i <= i );

    REQUIRE( i == i );
    REQUIRE( not (s == i) );

    REQUIRE( s != i );
    REQUIRE( not (i != i) );

    REQUIRE( i >= s );
    REQUIRE( not (s >= i) );
    REQUIRE( i >= i );

    REQUIRE( i > s );
    REQUIRE( not (s > i) );
}
