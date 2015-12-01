#include "catch.hpp"
#include "symcxx/core.hpp"

TEST_CASE( "homogenous ordering", "[symcxx::Symbol]" ) {
    auto ns = symcxx::NameSpace(2);
    auto& s0 = ns.instances[0];
    auto& s1 = ns.instances[1];

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

TEST_CASE( "heterogeneous ordering", "[symcxx::Basic]" ) {
    auto ns = symcxx::NameSpace(1);
    auto idx_i = ns.make_integer(0);
    auto& s = ns.instances[0];
    auto& i = ns.instances[idx_i];

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

TEST_CASE( "evalb", "[symcxx::Basic]" ) {
    auto ns = symcxx::NameSpace(1);
    auto idx_i = ns.make_integer(0);
    auto& s = ns.instances[0];
    auto& i = ns.instances[idx_i];

    std::vector<symcxx::idx_t> v{{0, 1}};
    auto idx_lt = ns.lt(v);
    auto idx_le = ns.le(v);
    auto idx_eq = ns.eq(v);
    auto idx_ne = ns.ne(v);
    auto idx_ge = ns.ge(v);
    auto idx_gt = ns.gt(v);

    auto lt = ns.instances[idx_lt];
    auto le = ns.instances[idx_le];
    auto eq = ns.instances[idx_eq];
    auto ne = ns.instances[idx_ne];
    auto ge = ns.instances[idx_ge];
    auto gt = ns.instances[idx_gt];

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
