#include <cassert>
#include <iostream>
#include "symcxx/core.hpp"

int main(){
    const double x[2] = {3, 5};
    auto ns = symcxx::NameSpace(2);
    std::vector<symcxx::idx_t> v {{ 0, 1, 1 }};
    auto expr_id = ns.add(v);
    std::cout << "About to differentiate:" << std::endl;

    auto diff1_id = ns.diff(expr_id, 1);
    const double res1 = ns.evalf(diff1_id, x);
    const double ref1 = 2;
    assert (std::abs(res1 - ref1) < 1e-15);
}
