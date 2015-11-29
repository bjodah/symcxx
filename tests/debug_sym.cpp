#include <cassert>
#include "sym.hpp"
#include <iostream>

int main(){
    double x0 = 0;
    double x1 = 1;
    double x2 = 2;
    double x3 = 3;
    auto ns = sym::NameSpace(1);
    std::vector<sym::idx_t> v{{0}};
    auto exp_id = ns.exp(v);
    std::cout << "x0" << std::endl;
    assert (std::abs(ns.evalf(exp_id, &x0) - std::exp(x0)) < 1e-15);
    std::cout << "x1" << std::endl;
    assert (std::abs(ns.evalf(exp_id, &x1) - std::exp(x1)) < 1e-15);
    std::cout << "x2" << std::endl;
    assert (std::abs(ns.evalf(exp_id, &x2) - std::exp(x2)) < 1e-15);
    std::cout << "x3" << std::endl;
    assert (std::abs(ns.evalf(exp_id, &x3) - std::exp(x3)) < 1e-15);
}
