#include "catch.hpp"
#include "symcxx/core.hpp"

TEST_CASE( "homogenous ordering", "[symcxx::Symbol]" ) {
    auto ns = symcxx::NameSpace(2);
    auto& s0 = ns.instances[0];
    auto& s1 = ns.instances[1];
    REQUIRE( s0.data.idx_pair.first == 0 );
    REQUIRE( s1.data.idx_pair.first == 1 );
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
    std::cout << "evalb" << std::endl;
    auto ns = symcxx::NameSpace(1);
    auto idx_i = ns.make_integer(0);
    auto idx_i2 = ns.make_integer(0);
    REQUIRE( idx_i == idx_i2 );

    auto idx_lt = ns.lt(0, idx_i);
    auto idx_le = ns.le(0, idx_i);
    auto idx_eq = ns.eq(0, idx_i);
    auto idx_ne = ns.ne(0, idx_i);
    auto idx_ge = ns.ge(0, idx_i);
    auto idx_gt = ns.gt(0, idx_i);

    auto& s = ns.instances[0];
    REQUIRE( s.data.idx_pair.first == 0 );
    auto& i = ns.instances[idx_i];
    REQUIRE( i.data.intgr == 0 );

    auto& lt = ns.instances[idx_lt];
    auto& le = ns.instances[idx_le];
    auto& eq = ns.instances[idx_eq];
    auto& ne = ns.instances[idx_ne];
    auto& ge = ns.instances[idx_ge];
    auto& gt = ns.instances[idx_gt];

    const double x1 = 1.0;
    const double xm1 = -1.0;
    const double x0 = 0.0;
    const double s_eval = s.evalf(&x1);

    REQUIRE( (s_eval - 1.0) == 0 );

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


TEST_CASE( "add_ordering", "[symcxx::Basic]" ) {
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
