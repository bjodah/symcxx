#include "sym.hpp"


sym::idx_t sym::NameSpace::diff(const Basic& inst, const Basic& wrt)
{
    std::vector<idx_t> objs;
#if !defined(NDEBUG)
    std::cout << "inst=" << &inst << std::endl;
    std::cout << "kind=" << static_cast<int>(inst.kind) << std::endl; // << ", idx=" << idx(&inst) << ", wrt=" << idx(&wrt) << std::endl;
#endif
    switch(inst.kind){
    case Kind::Symbol:
        if (inst == wrt)
            return make_integer(1);
        else
            return make_integer(0);
    case Kind::Integer:
        return make_integer(0);
    case Kind::Float:
        return make_float(0);
    case Kind::Add:
        for (const auto basic_ptr : inst.args()){
            std::cout << "basic_ptr=" << basic_ptr << std::endl;
            objs.push_back(diff(*basic_ptr, wrt));
        }
        return create(Kind::Add, objs);
    default:
        throw std::runtime_error("diff does not support kind.");
    }
}
