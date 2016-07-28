#include "catch.hpp"
#include "symcxx/core.hpp"
#include <cmath>

TEST_CASE( "construct, mul, evalf", "[symcxx::NameSpace]" ) {
    auto ns = symcxx::NameSpace();
    auto v = ns.make_symbols(3);
    auto mul_id = ns.mul(v);

    double data[3] = {2.0, 3.0, 5.0};
    double result = ns.evalf(mul_id, data);

    REQUIRE (std::abs(result - 30) < 1e-15);
}

TEST_CASE( "evalf", "[symcxx::NameSpace]" ) {
    double x0 = 0;
    double x1 = 1;
    double x2 = 2;
    double x3 = 3;
    auto ns = symcxx::NameSpace();
    auto x0_id = ns.make_symbol();

    REQUIRE (std::abs(ns.evalf(x0_id, &x0) - x0) < 1e-15);
    REQUIRE (std::abs(ns.evalf(x0_id, &x1) - x1) < 1e-15);
    REQUIRE (std::abs(ns.evalf(x0_id, &x2) - x2) < 1e-15);
    REQUIRE (std::abs(ns.evalf(x0_id, &x3) - x3) < 1e-15);

    auto exp_id = ns.exp(x0_id);
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
    auto sin_idx = ns.sin(x0_id);
    for (int i=0; i<5; ++i){
        res[i] = ns.evalf(sin_idx, x+i);
    }
    for (int i=0; i<5; ++i){
        REQUIRE (std::abs(res[i] - std::sin(x[i])) < 1e-15);
    }
}

TEST_CASE( "create", "[symcxx::NameSpace]" ) {
    auto ns = symcxx::NameSpace();
    auto two_id = ns.make_integer(2);
    auto two_plus_two_id = ns.create(symcxx::Kind::Add, two_id, two_id);
    REQUIRE (ns.instances[two_plus_two_id].kind == symcxx::Kind::Pow );
    REQUIRE (ns.instances[two_plus_two_id].data.idx_pair.first == two_id );
    REQUIRE (ns.instances[two_plus_two_id].data.idx_pair.second == two_id );
}


TEST_CASE( "diff_add", "[symcxx::NameSpace]" ) {
    const double x[2] = {3, 5};
    auto ns = symcxx::NameSpace();
    auto ids = ns.make_symbols(2);
    std::vector<symcxx::idx_t> v {{ ids[0], ids[1], ids[1] }};
    auto expr_id = ns.add(v);
    auto expr_id2 = ns.add(v);
    REQUIRE( expr_id == expr_id2 );
    const double res = ns.evalf(expr_id, x);
    const double ref = 3 + 5 + 5;
    REQUIRE (std::abs(res - ref) < 1e-15);

    auto diff0_id = ns.diff(expr_id, ids[0]);
    const double res0 = ns.evalf(diff0_id, x);
    const double ref0 = 1;
    REQUIRE (std::abs(res0 - ref0) < 1e-15);

    auto diff1_id = ns.diff(expr_id, ids[1]);
    const double res1 = ns.evalf(diff1_id, x);
    const double ref1 = 2;
    REQUIRE (std::abs(res1 - ref1) < 1e-15);

}

TEST_CASE( "diff_add2", "[symcxx::NameSpace]" ) {
    const double x[2] = {3, 5};
    auto ns = symcxx::NameSpace();
    auto symb_ids = ns.make_symbols(2);

    auto expr3_id = ns.add2(symb_ids[0], symb_ids[1]);
    const double res3 = ns.evalf(expr3_id, x);
    const double ref3 = 3 + 5;
    REQUIRE( std::abs(res3 - ref3) < 1e-15 );

    auto diff3_id = ns.diff(expr3_id, symb_ids[0]);

    const double res3d = ns.evalf(diff3_id, x);
    REQUIRE( std::abs(res3d - 1) < 1e-15 );
}

TEST_CASE( "diff_pow_00", "[symcxx::NameSpace]" ) {
    const double x[2] = {3, 5};
    auto ns = symcxx::NameSpace();
    auto symb_ids = ns.make_symbols(2);

    auto expr0_id = ns.pow(symb_ids[0], symb_ids[1]);
    const double res0 = ns.evalf(expr0_id, x);
    const double ref0 = pow(3, 5);
    REQUIRE( std::abs(res0 - ref0) < 1e-15 );

    auto diff0_id = ns.diff(expr0_id, symb_ids[0]);
    const double res0d = ns.evalf(diff0_id, x);
    REQUIRE( std::abs(res0d - 5*(3*3*3*3)) < 1e-14 );
}

TEST_CASE( "diff_mul2", "[symcxx::NameSpace]" ) {
    const double x[2] = {3, 5};
    auto ns = symcxx::NameSpace();
    auto pi_id = ns.make_float(3.14);
    auto x0_id = ns.make_symbol();
    auto add_id = ns.add2(x0_id, pi_id);
    auto x1_id = ns.make_symbol();
    auto mul_id = ns.mul2(x1_id, add_id);

    const double res0 = ns.evalf(mul_id, x);
    const double ref0 = (3+3.14)*5;
    REQUIRE( std::abs(res0 - ref0) < 1e-15 );

    auto diff0_id = ns.diff(mul_id, x0_id);
    const double res0d = ns.evalf(diff0_id, x);
    REQUIRE( std::abs(res0d - 5) < 1e-15 );

    auto diff1_id = ns.diff(mul_id, x1_id);
    const double res1d = ns.evalf(diff1_id, x);
    REQUIRE( std::abs(res1d - 6.14) < 1e-15 );

    auto intgr3 = ns.make_integer(3);
    auto expr_id = ns.mul2(intgr3, x0_id);
    auto dexpr_id = ns.diff(expr_id, x0_id);
    REQUIRE( dexpr_id == intgr3 );
}

TEST_CASE( "diff_div0", "[symcxx::NameSpace]" ) {
    const double x[1] = {3.14};
    auto ns = symcxx::NameSpace();
    symcxx::idx_t x0_id = ns.make_symbol(0);
    REQUIRE( ns.instances[x0_id].data.idx_pair.first == 0 );
    symcxx::idx_t zero_id = ns.make_integer(0);
    symcxx::idx_t one_id = ns.make_integer(1);

    auto div_id0 = ns.create(symcxx::Kind::Div, zero_id, x0_id);
    REQUIRE( div_id0 == zero_id );

    auto div_id1 = ns.create(symcxx::Kind::Div, x0_id, one_id);
    REQUIRE( div_id1 == x0_id );
}


TEST_CASE( "diff_div1", "[symcxx::NameSpace]" ) {
    const double x[1] = {3.14};
    auto ns = symcxx::NameSpace();
    symcxx::idx_t x0_id = ns.make_symbol(0);

    auto mul_id = ns.mul2(ns.make_integer(3), x0_id);
    auto add_id = ns.add2(mul_id, ns.make_integer(1));
    auto sub_id = ns.sub(x0_id, ns.make_integer(2));
    auto div_id = ns.div(add_id, sub_id);

    const double ref_div = (3*3.14 + 1) / (3.14 - 2);
    const double res_div = ns.evalf(div_id, x);
    REQUIRE( std::abs(res_div - ref_div) < 1e-15 );

    auto div_diff_id = ns.diff(div_id, x0_id);
    for (symcxx::idx_t i=0; i<ns.instances.size(); ++i)
        std::cout << i << ": " << ns.print_ast(i, {"x"}) << std::endl;

    const double ref_div_diff = (3*(3.14-2) - (3*3.14 + 1)) / ((3.14 - 2)*(3.14 - 2));
    const double res_div_diff = ns.evalf(div_diff_id, x);
    REQUIRE( std::abs(res_div_diff - ref_div_diff) < 1e-15 );
}

TEST_CASE( "diff_log1", "[symcxx::NameSpace]" ) {
    const double x[1] = {3.14};
    auto ns = symcxx::NameSpace();
    symcxx::idx_t x0_id = ns.make_symbol();

    auto mul_id = ns.mul2(ns.make_integer(3), x0_id);
    auto add_id = ns.add2(mul_id, ns.make_integer(1));
    auto log_id = ns.log(add_id);

    const double ref_log = std::log(3*3.14 + 1);
    const double res_log = ns.evalf(log_id, x);
    REQUIRE( std::abs(res_log - ref_log) < 1e-15 );

    std::cout << "\n\nAbout to diff!\n\n";
    auto log_diff_id = ns.diff(log_id, x0_id);
    std::cout << "\n\nDiff done!\n\n";
    for (symcxx::idx_t i=0; i<ns.instances.size(); ++i)
        std::cout << i << ": " << ns.print_ast(i, {"x"}) << std::endl;

    const double ref_log_diff = 3./(3*3.14 + 1);
    const double res_log_diff = ns.evalf(log_diff_id, x);
    REQUIRE( std::abs(res_log_diff - ref_log_diff) < 1e-15 );
}

TEST_CASE( "matrix_evalf", "[symcxx::NameSpace]" ) {
    const double x[2] = {3.14, 42.0};
    double xout[2];
    auto ns = symcxx::NameSpace();
    auto x0_id = ns.make_symbol();
    auto x1_id = ns.make_symbol();
    auto one_id = ns.make_integer(1);
    auto two_id = ns.make_integer(2);
    auto sub_id = ns.sub(x1_id, x0_id);
    auto div_id = ns.div(x0_id, x1_id);
    auto mtx_id = ns.make_matrix(two_id, one_id, { sub_id, div_id });
    ns.matrix_evalf(mtx_id, x, xout);
    REQUIRE( std::abs(xout[0] - (x[1] - x[0])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[0] / x[1])) < 1e-15 );
}

TEST_CASE( "matrix_jacobian_simple", "[symcxx::NameSpace]" ) {
    const double x[1] = {3.14};
    double xout[1];
    auto ns = symcxx::NameSpace();
    auto x_id = ns.make_symbol();
    auto mul_id = ns.mul2(x_id, x_id);
    auto one_id = ns.make_integer(1);
    auto mtx_id = ns.make_matrix(one_id, one_id, { mul_id });  // [x**2]
    auto wrt_id = ns.make_matrix(one_id, one_id, { x_id }); // [[x]]
    auto jac_id = ns.matrix_jacobian(mtx_id, wrt_id); // [[ 2*x ]]
    std::cout << ns.print_ast(mtx_id, {"x"});
    std::cout << ns.print_ast(wrt_id, {"x"});
    std::cout << ns.print_ast(jac_id, {"x"});
    ns.matrix_evalf(mtx_id, x, xout);
    REQUIRE( std::abs(xout[0] - x[0]*x[0]) < 1e-15 );
    ns.matrix_evalf(wrt_id, x, xout);
    REQUIRE( std::abs(xout[0] - x[0]) < 1e-15 );
    ns.matrix_evalf(jac_id, x, xout);
    REQUIRE( std::abs(xout[0] - 2*x[0]) < 1e-15 );
}

TEST_CASE( "matrix_jacobian1", "[symcxx::NameSpace]" ) {
    const double x[2] = {3.14, 42.0};
    double xout[4];
    auto ns = symcxx::NameSpace();
    auto x0_id = ns.make_symbol();
    auto x1_id = ns.make_symbol();
    auto one_id = ns.make_integer(1);
    auto two_id = ns.make_integer(2);
    auto sub_id = ns.sub(x1_id, x0_id);
    auto div_id = ns.div(x0_id, x1_id);
    auto mtx_id = ns.make_matrix(two_id, one_id, { sub_id, div_id });  // [y-x, x/y]
    auto wrt_id = ns.make_matrix(two_id, one_id, { x0_id, x1_id }); // [x, y]
    auto jac_id = ns.matrix_jacobian(mtx_id, wrt_id);
    // [[ -1,  1 ],
    //  [ 1/y, -x/(y*y) ]
    ns.matrix_evalf(jac_id, x, xout);
    REQUIRE( std::abs(xout[0] - (-1)) < 1e-15 );
    REQUIRE( std::abs(xout[1] - ( 1)) < 1e-15 );
    REQUIRE( std::abs(xout[2] - (1.0/x[1])) < 1e-15 );
    REQUIRE( std::abs(xout[3] - (-x[0]/(x[1]*x[1]))) < 1e-15 );
}

TEST_CASE( "matrix_jacobian2", "[symcxx::NameSpace]" ) {
    const double x[2] = {7, 11};
    double xout[4];
    auto ns = symcxx::NameSpace();
    auto x0_id = ns.make_symbol();
    auto x1_id = ns.make_symbol();
    auto one_id = ns.make_integer(1);
    auto two_id = ns.make_integer(2);
    auto three_id = ns.make_integer(3);
    auto x3_id = ns.pow(x0_id, three_id);
    auto xp1_id = ns.add2(x0_id, one_id);
    auto yp1_id = ns.add2(x1_id, one_id);
    auto x3muly_id = ns.mul2(x3_id, x1_id);
    auto xp1mulyp1_id = ns.mul2(xp1_id, yp1_id);
    // [x**3 * y, (x+1)*(y+1)]
    auto mtx_id = ns.make_matrix(two_id, one_id, { x3muly_id, xp1mulyp1_id });
    auto wrt_id = ns.make_matrix(two_id, one_id, { x0_id, x1_id }); // [x, y]
    auto jac_id = ns.matrix_jacobian(mtx_id, wrt_id);
    // [[3 * X**2 * Y, X**3],
    //  [Y + 1, X + 1]])
    ns.matrix_evalf(jac_id, x, xout);
    REQUIRE( std::abs(xout[0] - (3*x[0]*x[0]*x[1])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[0]*x[0]*x[0])) < 1e-15 );
    REQUIRE( std::abs(xout[2] - (x[1] + 1)) < 1e-15 );
    REQUIRE( std::abs(xout[3] - (x[0] + 1)) < 1e-15 );
}

TEST_CASE( "rebuild_from_matrix2", "[symcxx::NameSpace]" ) {
    const double x[2] = {7, 11};
    double xout[4];
    auto ns = symcxx::NameSpace();
    auto x0_id = ns.make_symbol();
    auto x1_id = ns.make_symbol();
    auto one_id = ns.make_integer(1);
    auto two_id = ns.make_integer(2);
    auto three_id = ns.make_integer(3);
    auto x3_id = ns.pow(x0_id, three_id);
    auto xp1_id = ns.add2(x0_id, one_id);
    auto yp1_id = ns.add2(x1_id, one_id);
    auto x3muly_id = ns.mul2(x3_id, x1_id);
    auto xp1mulyp1_id = ns.mul2(xp1_id, yp1_id);
    // [x**3 * y, (x+1)*(y+1)]
    auto mtx_id = ns.make_matrix(two_id, one_id, { x3muly_id, xp1mulyp1_id });
    auto wrt_id = ns.make_matrix(two_id, one_id, { x0_id, x1_id }); // [x, y]
    auto jac_id = ns.matrix_jacobian(mtx_id, wrt_id);
    // [[3 * X**2 * Y, X**3],
    //  [Y + 1, X + 1]])

    auto ns2 = ns.rebuild_from_matrix({x0_id, x1_id}, jac_id);
    ns2->matrix_evalf(ns2->instances.size() - 1, x, xout);
    REQUIRE( std::abs(xout[0] - (3*x[0]*x[0]*x[1])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[0]*x[0]*x[0])) < 1e-15 );
    REQUIRE( std::abs(xout[2] - (x[1] + 1)) < 1e-15 );
    REQUIRE( std::abs(xout[3] - (x[0] + 1)) < 1e-15 );
}

TEST_CASE( "rebuild_from_matrix", "[symcxx::NameSpace]" ) {
    // begin copy-paste from matrix_evalf
    const double x[3] = {3.14, 42.0, 13.0};
    double xout[3];
    auto ns = symcxx::NameSpace();
    auto x0_id = ns.make_symbol();
    auto x1_id = ns.make_symbol();
    auto one_id = ns.make_integer(1);
    auto two_id = ns.make_integer(2);
    auto sub_id = ns.sub(x1_id, x0_id);
    auto div_id = ns.div(x0_id, x1_id);
    auto mtx_id = ns.make_matrix(two_id, one_id, { sub_id, div_id });
    ns.matrix_evalf(mtx_id, x, xout);
    REQUIRE( std::abs(xout[0] - (x[1] - x[0])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[0] / x[1])) < 1e-15 );
    // end copy-paste

    auto ns2 = ns.rebuild_from_matrix({x1_id, x0_id}, mtx_id);
    ns2->matrix_evalf(ns2->instances.size() - 1, x, xout);
    REQUIRE( std::abs(xout[0] - (x[0] - x[1])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[1] / x[0])) < 1e-15 );

    auto decr_id = ns.sub(x1_id, one_id);
    auto mtx2_id = ns.make_matrix(two_id, one_id, { decr_id, sub_id });
    ns.matrix_evalf(mtx2_id, x, xout);
    REQUIRE( std::abs(xout[0] - (x[1] - 1)) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[1] - x[0])) < 1e-15 );
    auto ns3 = ns.rebuild_from_matrix({x0_id, x1_id}, mtx2_id);
    ns3->matrix_evalf(ns3->instances.size() - 1, x, xout);
    REQUIRE( std::abs(xout[0] - (x[1] - 1)) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[1] - x[0])) < 1e-15 );

    auto x2_id = ns.make_symbol();
    auto incr_id = ns.add2(x2_id, one_id);
    auto mtx3_id = ns.make_matrix(two_id, one_id, { div_id, incr_id });
    ns.matrix_evalf(mtx3_id, x, xout);
    REQUIRE( std::abs(xout[0] - (x[0] / x[1])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[2] + 1)) < 1e-15 );
    auto ns4 = ns.rebuild_from_matrix({x0_id, x1_id, x2_id}, mtx3_id);
    ns4->matrix_evalf(ns4->instances.size() - 1, x, xout);
    REQUIRE( std::abs(xout[0] - (x[0] / x[1])) < 1e-15 );
    REQUIRE( std::abs(xout[1] - (x[2] + 1)) < 1e-15 );

    auto mul_id = ns.mul2(x0_id, x0_id);
    auto mtx4_id = ns.make_matrix(one_id, one_id, { mul_id });
    ns.matrix_evalf(mtx4_id, x, xout);
    REQUIRE( std::abs(xout[0] - (x[0] * x[0])) < 1e-15 );
    auto ns5 = ns.rebuild_from_matrix({x0_id}, mtx4_id);
    ns5->matrix_evalf(ns5->instances.size() - 1, x, xout);
    REQUIRE( std::abs(xout[0] - (x[0] * x[0])) < 1e-15 );
}

TEST_CASE( "prime_sieve", "[symcxx::NameSpace]" ) {
    auto ns = symcxx::NameSpace();
    REQUIRE( ns.prime_sieve.is_prime(1) );
    REQUIRE( ns.prime_sieve.is_prime(2) );
    REQUIRE( ns.prime_sieve.is_prime(3) );
    REQUIRE( ns.prime_sieve.is_prime(5) );
    REQUIRE( ns.prime_sieve.is_prime(7) );
    REQUIRE( ns.prime_sieve.is_prime(11) );
    REQUIRE( ns.prime_sieve.is_prime(13) );

    REQUIRE( !ns.prime_sieve.is_prime(4) );
    REQUIRE( !ns.prime_sieve.is_prime(6) );
    REQUIRE( !ns.prime_sieve.is_prime(8) );
    REQUIRE( !ns.prime_sieve.is_prime(9) );
    REQUIRE( !ns.prime_sieve.is_prime(10) );
    REQUIRE( !ns.prime_sieve.is_prime(12) );
    REQUIRE( !ns.prime_sieve.is_prime(14) );
}
