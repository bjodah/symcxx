#include "catch.hpp"
#include "sym.hpp"

TEST_CASE( "construct, mul, evalf", "[sym::NameSpace]" ) {
    auto ns = sym::NameSpace(3);
    std::vector<sym::idx_t> v{{0, 1, 2}};
    auto mul_id = ns.mul(v);
    REQUIRE( mul_id == 3 );

    double data[3] = {2.0, 3.0, 5.0};
    double result = ns.evalf(mul_id, data);

    REQUIRE (std::abs(result - 30) < 1e-15);
}
