#include "symcxx/core.hpp"

symcxx::NameSpace::NameSpace(idx_t nsymbs) : nsymbs(nsymbs) {
    for (idx_t idx=0; idx < nsymbs; ++idx)
        instances.push_back(Symbol(idx, this));
}

symcxx::idx_t
symcxx::NameSpace::idx(const Basic* basic_ptr) const {
    const auto begin = &instances[0];
    const auto end = begin + instances.capacity();
    if ((basic_ptr < begin) || (basic_ptr > end))
        throw std::runtime_error("Instance not in namespace.");
    return (basic_ptr - begin);
}

symcxx::idx_t
symcxx::NameSpace::reg_args(const std::vector<idx_t>& objs) {
    args_stack.push_back(objs);
    return args_stack.size() - 1;
}

bool
symcxx::NameSpace::has(const Basic& looking_for, idx_t * idx) const {
    idx_t idx_ = 0;
    for (const auto& inst : instances){
        if (looking_for == inst){
            *idx = idx_;
            return true;
        }
        ++idx_;
    }
    return false;
}

symcxx::idx_t
symcxx::NameSpace::make_integer(int i){
    const auto instance = Integer(i, this);
    idx_t idx;
    if (has(instance, &idx)){
        std::cout << "make_integer(" << i << ") - old!" << std::endl;
        return idx;
    }
    std::cout << "make_integer(" << i << ") - new!" << std::endl;
    instances.push_back(instance);
    return instances.size() - 1;
}

symcxx::idx_t
symcxx::NameSpace::make_float(double f){
    const auto instance = Float(f, this);
    idx_t idx;
    if (has(instance, &idx))
        return idx;
    instances.push_back(instance);
    return instances.size() - 1;
}

#define METH(Name, Constr) \
    symcxx::idx_t symcxx::NameSpace::Name(const std::vector<symcxx::idx_t>& objs){ \
        const auto instance = Constr(reg_args(objs), this);             \
        idx_t idx;                                                      \
        if (has(instance, &idx)){                                       \
            std::cout << SX(Name) "(...) - old!" << std::endl;          \
            args_stack.pop_back();                                      \
            return idx;                                                 \
        }                                                               \
        std::cout << SX(Name) "(...) - new!" << std::endl;              \
        instances.push_back(instance);                                  \
        return instances.size() - 1;                                    \
    }
#define SYMCXX_TYPE(Cls, Parent, meth) METH(meth, Cls)
#include "symcxx/types_composed.inc"
#undef SYMCXX_TYPE
#undef METH


symcxx::idx_t
symcxx::NameSpace::create(const Kind kind, const std::vector<idx_t>& objs){
    switch(kind){
    case Kind::Add:
        return add(objs);
    default:
        throw std::runtime_error("create does not support kind.");
    }
}
double
symcxx::NameSpace::evalf(const idx_t id, const double inp[]) const {
    return instances[id].evalf(inp);
}

symcxx::idx_t
symcxx::NameSpace::diff(const idx_t inst_id, const idx_t wrt_id)
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
            objs.push_back(diff(idx, wrt_id));
        }
        return create(Kind::Add, objs);
    default:
        throw std::runtime_error("diff does not support kind.");
    }
}
