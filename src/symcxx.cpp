#include "symcxx/core.hpp"

symcxx::idx_t symcxx::NameSpace::diff(const idx_t inst_id, const idx_t wrt_id)
{
    const Basic& inst = instances[inst_id];
    const Basic& wrt = instances[wrt_id];
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
        for (const auto idx : inst.args()){
            std::cout << "idx=" << idx << std::endl;
            objs.push_back(diff(idx, wrt));
        }
        return create(Kind::Add, objs);
    default:
        throw std::runtime_error("diff does not support kind.");
    }
}
