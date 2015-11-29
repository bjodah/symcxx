#include "sym.hpp"

sym::hash_t sym::calc_hash(const BasicCont * const data, const Kind kind) {
    const sym::hash_t most_significant = static_cast<sym::hash_t>(1) << (8*sizeof(sym::hash_t) - 1);
    sym::hash_t result = most_significant >> static_cast<int>(kind);
    for (const auto basic_ptr : *data)
        result = result ^ basic_ptr->hash;
    return result;
};

bool sym::lt(const data_t arg1, const data_t arg2, const Kind kind){
    switch(kind){
    case Kind::Symbol:
        return arg1.id < arg2.id;
    case Kind::Integer:
        return arg1.i < arg2.i;
    case Kind::Float:
        return arg1.d < arg2.d;
    default:
        break;
    }
    auto& c1 = SYM_BCAST(arg1.b);
    auto& c2 = SYM_BCAST(arg2.b);
    if (c1.size() != c2.size())
        return c1.size() < c2.size();
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = *c1[idx];
        const auto& e2 = *c2[idx];
        if (e1.kind != e2.kind)
            return e1.kind < e2.kind;
        if (!(e1 == e2))
            return e1 < e2;
    }
    return false;
}
bool sym::eq(const data_t arg1, const data_t arg2, const Kind kind){
    switch(kind){
    case Kind::Symbol:
        return arg1.id == arg2.id;
    case Kind::Integer:
        return arg1.i == arg2.i;
    case Kind::Float:
        return arg1.d == arg2.d;
    default:
        break;
    }
    auto& c1 = SYM_BCAST(arg1.b);
    auto& c2 = SYM_BCAST(arg2.b);
    if (c1.size() != c2.size())
        return false;
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = *c1[idx];
        const auto& e2 = *c2[idx];
        if (!(e1 == e2))
            return false;
    }
    return true;
}
