#include "catch.hpp"
#include "sym.hpp"
#include "cmath"

TEST_CASE( "construct, mul, evalf", "[sym::NameSpace]" ) {
    auto ns = sym::NameSpace(3);
    std::vector<sym::idx_t> v{{0, 1, 2}};
    auto mul_id = ns.mul(v);
    REQUIRE( mul_id == 3 );

    double data[3] = {2.0, 3.0, 5.0};
    double result = ns.evalf(mul_id, data);

    REQUIRE (std::abs(result - 30) < 1e-15);
}

TEST_CASE( "evalf of functions", "[sym::NameSpace]" ) {
    double x0 = 0;
    double x1 = 1;
    double x2 = 2;
    auto ns = sym::NameSpace(1);
    auto exp_id = ns.exp(0);
    REQUIRE (std::abs(ns.evalf(0, &x0) - std::exp(x0)) < 1e-15);
    REQUIRE (std::abs(ns.evalf(0, &x1) - std::exp(x1)) < 1e-15);
    REQUIRE (std::abs(ns.evalf(0, &x2) - std::exp(x2)) < 1e-15);
}
