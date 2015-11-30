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

TEST_CASE( "evalf", "[sym::NameSpace]" ) {
    double x0 = 0;
    double x1 = 1;
    double x2 = 2;
    double x3 = 3;
    auto ns = sym::NameSpace(1);

    REQUIRE (std::abs(ns.evalf(0, &x0) - x0) < 1e-15);
    REQUIRE (std::abs(ns.evalf(0, &x1) - x1) < 1e-15);
    REQUIRE (std::abs(ns.evalf(0, &x2) - x2) < 1e-15);
    REQUIRE (std::abs(ns.evalf(0, &x3) - x3) < 1e-15);

    std::vector<sym::idx_t> v{{0}};
    auto exp_id = ns.exp(v);
    const double exp0 = ns.evalf(exp_id, &x0);
    const double exp1 = ns.evalf(exp_id, &x1);
    const double exp2 = ns.evalf(exp_id, &x2);
    const double exp3 = ns.evalf(exp_id, &x3);
    REQUIRE (std::abs(exp0 - std::exp(x0)) < 1e-15);
    REQUIRE (std::abs(exp1 - std::exp(x1)) < 1e-15);
    REQUIRE (std::abs(exp2 - std::exp(x2)) < 1e-15);
    REQUIRE (std::abs(exp3 - std::exp(x3)) < 1e-15);

    double x[5] = {-2, -1, 0, 1, 2};
    double res[5];
    auto sin_idx = ns.sin(v);
    for (int i=0; i<5; ++i){
        res[i] = ns.evalf(sin_idx, x+i);
    }
    for (int i=0; i<5; ++i){
        REQUIRE (std::abs(res[i] - std::sin(x[i])) < 1e-15);
    }
}

TEST_CASE( "diff_add", "[sym::NameSpace]" ) {
    auto ns = sym::NameSpace(2);
    std::vector<sym::idx_t> v {{ 0, 1, 1 }};
    auto expr_id = ns.add(v);
    const double x[2] = {3, 5};
    const double res = ns.evalf(expr_id, x);
    const double ref = 3 + 5 + 5;
    // REQUIRE (std::abs(res - ref) < 1e-15);

    auto diff0_id = ns.diff(expr_id, 0);
    const double res0 = ns.evalf(diff0_id, x);
    const double ref0 = 1;
    REQUIRE (std::abs(res0 - ref0) < 1e-15);

    auto diff1_id = ns.diff(expr_id, 1);
    const double res1 = ns.evalf(diff0_id, x);
    const double ref1 = 2;
    REQUIRE (std::abs(res1 - ref1) < 1e-15);
}
