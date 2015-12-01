#pragma once

#include <cmath>
#include <functional>
#include <vector>

#if !defined(NDEBUG)
#include <iostream>
#endif

namespace symcxx{
    using hash_t = int; //std::size_t;
    using idx_t = std::size_t;
    union data_t {
        const idx_t id;
        const int i; // change to int64_t
        const double d;
        data_t(const idx_t id) : id(id) {}
        data_t(const int i) : i(i) {} // change to int64_t
        data_t(const double d) : d(d) {}
    };

    enum class Kind : int
    {
#define SYMCXX_TYPE(Cls, Parent, meth) Cls,
#include "symcxx/types.inc"
#undef SYMCXX_TYPE
        Kind_Count
    };

    struct Basic;
    struct NameSpace;
#define SYMCXX_TYPE(Cls, Parent, meth) struct Cls;
#include "symcxx/types_composed.inc"
#undef SYMCXX_TYPE


    using ArgStack_t = std::vector<std::vector<idx_t> >;

    hash_t calc_hash(const idx_t args_idx, const Kind kind,
                     const ArgStack_t& args_stack, const std::vector<Basic>& instances);
    bool lt(const data_t arg1, const data_t arg2, const Kind kind, const ArgStack_t&, const std::vector<Basic>&);
    bool eq(const data_t arg1, const data_t arg2, const Kind kind, const ArgStack_t&, const std::vector<Basic>&);
}

#include "symcxx/basic.hpp"
#include "symcxx/namespace.hpp"
#include "symcxx/derived.hpp"

inline symcxx::hash_t symcxx::calc_hash(const idx_t args_idx, const Kind kind,
                                  const ArgStack_t& args_stack, const std::vector<Basic>& instances) {
    const symcxx::hash_t most_significant = static_cast<symcxx::hash_t>(1) << (8*sizeof(symcxx::hash_t) - 1);
    symcxx::hash_t result = most_significant >> static_cast<int>(kind);
    result = result ^ most_significant >> static_cast<int>(kind)/2;
    for (const auto idx : args_stack[args_idx])
        result = result ^ instances[idx].hash;
    return result;
}

inline bool symcxx::lt(const data_t arg1, const data_t arg2, const Kind kind,
                       const ArgStack_t& args_stack, const std::vector<Basic>& instances){
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
    auto& c1 = args_stack[arg1.id];
    auto& c2 = args_stack[arg2.id];
    if (c1.size() != c2.size())
        return c1.size() < c2.size();
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = instances[c1[idx]];
        const auto& e2 = instances[c2[idx]];
        if (e1.kind != e2.kind)
            return e1.kind < e2.kind;
        if (!(e1 == e2))
            return e1 < e2;
    }
    return false;
}

inline bool symcxx::eq(const data_t arg1, const data_t arg2, const Kind kind,
                       const ArgStack_t& args_stack, const std::vector<Basic>& instances){
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
    auto& c1 = args_stack[arg1.id];
    auto& c2 = args_stack[arg2.id];
    if (c1.size() != c2.size())
        return false;
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = instances[c1[idx]];
        const auto& e2 = instances[c2[idx]];
        if (!(e1 == e2))
            return false;
    }
    return true;
}
