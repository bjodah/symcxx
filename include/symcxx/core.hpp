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
        const void * const b;
        data_t(const idx_t id) : id(id) {}
        data_t(const int i) : i(i) {} // change to int64_t
        data_t(const double d) : d(d) {}
    };

    enum class Kind : int
    {
#define SYMCXX_TYPE(Cls, Parent, meth) Cls
#include "symcxx/types.inc"
#undef SYMCXX_TYPE
    };

    struct Basic;
    struct NameSpace;
    using ArgStack_t = std::vector<std::vector<idx_t> >;

    hash_t calc_hash(const idx_t args_idx, const Kind kind,
                     const ArgStack_t& args_stack, const std::vector<Basic>& instances);
    bool lt(const data_t arg1, const data_t arg2, const Kind kind, const ArgStack_t&, const std::vector<Basic>&);
    bool eq(const data_t arg1, const data_t arg2, const Kind kind, const ArgStack_t&, const std::vector<Basic>&);

    struct Composed : public Basic {
        Composed(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Basic(calc_hash(args_idx, kind, ns->args_stack, ns->instances), kind, args_idx) {}
    };

    struct Symbol : public Basic {
        Symbol(const idx_t data, const NameSpace * const ns) : Basic(std::hash<idx_t>()(data), Kind::Symbol, data, ns) {}
    };
    struct Float : public Basic {
        Float(const double data, const NameSpace * const ns) : Basic(std::hash<double>()(data), Kind::Float, data, ns) {}
    };
    struct Integer : public Basic {
        Integer(const int data,  const NameSpace * const ns) : Basic(std::hash<int>()(data), Kind::Integer, data, ns) {}
    };

    struct Unary : public Composed {
#if !defined(NDEBUG)
        Unary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns) {
            if (ns->args_stack[ns->args_idx].size() != 1)
                throw std::runtime_error("Unary takes one argument");
        }
#endif
    };
    struct Binary : public Composed {
#if !defined(NDEBUG)
        Binary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns) {
            if (ns->args_stack[ns->args_idx].size() != 2)
                throw std::runtime_error("Binary takes two arguments");
        }
#endif
    };
    struct Reduction : public Composed {
#if !defined(NDEBUG)
        Reduction(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns) {
            if (ns->args_stack[ns->args_idx].size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
        }
#endif
    };

    struct Ternary : public Composed {
#if !defined(NDEBUG)
        Ternary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns)
        {
            if (ns->args_stack[ns->args_idx].size() != 3)
                throw std::runtime_error("ITE needs three arguments");
            if (ns->instances[ns->args_stack[ns->args_idx][0]].kind < Kind::Lt)
                throw std::runtime_error("ITE arg0 needs to be a Relational");
            if (ns->instances[ns->args_stack[ns->args_idx][1]].kind >= Kind::Lt)
                throw std::runtime_error("ITE arg1 cannot be a Relational");
            if (ns->instances[ns->args_stack[ns->args_idx][2]].kind >= Kind::Lt)
                throw std::runtime_error("ITE arg2 cannot be a Relational");
        }
#endif
    };

#define DECLARE(Name, Base) struct Name : public Base {\
        Name(idx_t args_idx,  const NameSpace * const ns) : \
            Base(args_idx, Kind::Name, ns) {} \
    };
#define SYMCXX_TYPE(Cls, Parent, meth) DECLARE(Cls, Parent)
#include "symcxx/types_composed.inc"
#undef SYMCXX_TYPE
#undef DECLARE

}

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
