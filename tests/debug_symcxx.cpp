#include <cassert>
#include <iostream>
#include "symcxx/core.hpp"

int main(){
    double x0 = 0;
    auto ns = symcxx::NameSpace(1);
    std::vector<symcxx::idx_t> v{{0}};
    auto exp_id = ns.exp(v);
    const double res = ns.evalf(exp_id, &x0);
    assert (std::abs(res - std::exp(x0)) < 1e-15);

    // double x1 = 1;
    // double x2 = 2;
    // double x3 = 3;
    // assert (std::abs(ns.evalf(exp_id, &x1) - std::exp(x1)) < 1e-15);
    // assert (std::abs(ns.evalf(exp_id, &x2) - std::exp(x2)) < 1e-15);
    // assert (std::abs(ns.evalf(exp_id, &x3) - std::exp(x3)) < 1e-15);
}
