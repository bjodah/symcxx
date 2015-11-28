#include "sym.hpp"

std::size_t sym::calc_hash(BasicCont* data) {
    std::size_t result = 0;
    for (const auto& v : *data)
        result = result ^ SYM_GET(v).hash;
    return result;
};

bool sym::lt(data_t arg1, data_t arg2, Kind kind){
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
        auto& e1 = SYM_GET(c1[idx]);
        auto& e2 = SYM_GET(c2[idx]);
        if (e1.kind != e2.kind)
            return e1.kind < e2.kind;
        if (!(e1 == e2))
            return e1 < e2;
    }
    return false;
}
bool sym::eq(data_t arg1, data_t arg2, Kind kind){
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
        auto& e1 = SYM_GET(c1[idx]);
        auto& e2 = SYM_GET(c2[idx]);
        if (!(e1 == e2))
            return false;
    }
    return true;
}
