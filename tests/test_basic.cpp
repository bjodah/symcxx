#include "catch.hpp"
#include "symcxx/core.hpp"

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

TEST_CASE( "evalb", "[sym::Basic]" ) {
    auto s = sym::Symbol(0);
    auto i = sym::Integer(0);
    sym::BasicPtrVec args {{ &s, &i }};

    auto lt = sym::Lt(&args);
    auto le = sym::Le(&args);
    auto eq = sym::Eq(&args);
    auto ne = sym::Ne(&args);
    auto ge = sym::Ge(&args);
    auto gt = sym::Gt(&args);

    const double x1 = 1.0;
    const double xm1 = -1.0;
    const double x0 = 0.0;

    REQUIRE( (s.evalf(&x1) - 1.0) == 0 );

    REQUIRE( not lt.evalb(&x1) );
    REQUIRE( not lt.evalb(&x0) );
    REQUIRE( lt.evalb(&xm1) );

    REQUIRE( not le.evalb(&x1) );
    REQUIRE( le.evalb(&xm1) );
    REQUIRE( le.evalb(&x0) );

    REQUIRE( not eq.evalb(&x1) );
    REQUIRE( eq.evalb(&x0) );

    REQUIRE( not ne.evalb(&x0) );
    REQUIRE( ne.evalb(&x1) );

    REQUIRE( not ge.evalb(&xm1) );
    REQUIRE( ge.evalb(&x1) );
    REQUIRE( ge.evalb(&x0) );

    REQUIRE( not gt.evalb(&xm1) );
    REQUIRE( not gt.evalb(&x0) );
    REQUIRE( gt.evalb(&x1) );

}
