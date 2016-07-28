#pragma once

#include <algorithm>  // std::sort
#include <array>  // std::array
#include <cmath>
#include <functional>
#include <memory>  // std::unique_ptr
#include <vector>  // std::vector
#include <string>
#include <sstream>

// #if !defined(NDEBUG)
#include <iostream>
#include <iterator>
template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  if ( !v.empty() ) {
    out << '[';
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}
//#endif

namespace symcxx{
    using hash_t = int; // std::size_t;
    using idx_t = unsigned int; // std::size_t;
    using intgr_t = int64_t;
    struct idx_pair_t {
        idx_t first;
        idx_t second;
    };

    union data_t {
        const idx_pair_t idx_pair;
        const intgr_t intgr;
        const double dble;
        data_t(const idx_t id) : idx_pair({id, 0}) {}
        data_t(const idx_pair_t pr) : idx_pair(pr) {}
        data_t(const intgr_t i) : intgr(i) {}
        data_t(const double d) : dble(d) {}
    };
    static_assert(sizeof(data_t) == 8, "8 bytes please");  // mind the cache

    enum class Kind : int
    {
#define SYMCXX_TYPE(Cls, Parent, meth) Cls,
#include "symcxx/types.inc"
#undef SYMCXX_TYPE
        Kind_Count
    };

#define STRINGIFY0(x) #x
#define STRINGIFY(x) STRINGIFY0(x)

    const static std::array<std::string, static_cast<int>(Kind::Kind_Count)+1> kind_names {{
#define SYMCXX_TYPE(Cls, Parent, meth) STRINGIFY(Cls),
#include "symcxx/types.inc"
#undef SYMCXX_TYPE
        "Kind_Count"
            }};

    struct Basic;
    struct Matrix;
    struct NameSpace;
#define SYMCXX_TYPE(Cls, Parent, meth) struct Cls;
#include "symcxx/types_nonatomic.inc"
#undef SYMCXX_TYPE

    inline std::string all_kind_names(){
        std::string s;
        for (int i=0; i < static_cast<int>(Kind::Kind_Count); ++i)
            s += std::to_string(i) + ": " + kind_names[i] + "\n";
        return s;
    }

    using ArgStack_t = std::vector<std::vector<idx_t> >;

    // Reduction
    hash_t calc_hash(const idx_t args_idx, const Kind kind,
                     const std::vector<Basic>& instances, const ArgStack_t& args_stack);
    // Unary
    hash_t calc_hash(const idx_t inst_idx, const Kind kind,
                     const std::vector<Basic>& instances);
    // Binary
    hash_t calc_hash(const idx_t inst_idx0, const idx_t inst_idx1, const Kind kind,
                     const std::vector<Basic>& instances);

    bool lt(const data_t arg1, const data_t arg2, const Kind kind, const ArgStack_t&, const std::vector<Basic>&);
    bool eq(const data_t arg1, const data_t arg2, const Kind kind, const ArgStack_t&, const std::vector<Basic>&);
}


#include "symcxx/isprime.hpp"
#include "symcxx/basic.hpp"
#include "symcxx/namespace.hpp"
#include "symcxx/derived.hpp"
#include "symcxx/matrix.hpp"

inline symcxx::hash_t symcxx::calc_hash(const idx_t args_idx, const Kind kind,
                                        const std::vector<Basic>& instances,
                                        const ArgStack_t& args_stack) {
    const symcxx::hash_t most_significant = static_cast<symcxx::hash_t>(1) << (8*sizeof(symcxx::hash_t) - 1);
    symcxx::hash_t result = most_significant >> static_cast<int>(kind);
    result = result ^ most_significant >> static_cast<int>(kind)/2;
    for (const auto idx : args_stack[args_idx])
        result = result ^ instances[idx].hash;
    return result;
}

inline symcxx::hash_t symcxx::calc_hash(const idx_t inst_idx, const Kind kind,
                                        const std::vector<Basic>& instances) {
    const symcxx::hash_t most_significant = static_cast<symcxx::hash_t>(1) << (8*sizeof(symcxx::hash_t) - 1);
    symcxx::hash_t result = most_significant >> static_cast<int>(kind);
    result = result ^ most_significant >> static_cast<int>(kind)/2;
    result = result ^ instances[inst_idx].hash;
    return result;
}

inline symcxx::hash_t symcxx::calc_hash(const idx_t inst_idx0, const idx_t inst_idx1, const Kind kind,
                                        const std::vector<Basic>& instances) {
    const symcxx::hash_t most_significant = static_cast<symcxx::hash_t>(1) << (8*sizeof(symcxx::hash_t) - 1);
    symcxx::hash_t result = most_significant >> static_cast<int>(kind);
    result = result ^ most_significant >> static_cast<int>(kind)/2;
    result = result ^ instances[inst_idx0].hash;
    result = result ^ instances[inst_idx1].hash;
    return result;
}


// compare objects of same kind:
inline bool symcxx::lt(const data_t arg1, const data_t arg2, const Kind kind,
                       const ArgStack_t& args_stack, const std::vector<Basic>& instances){
    switch(kind){
    case Kind::Symbol:
        return arg1.idx_pair.first < arg2.idx_pair.first;
    case Kind::Integer:
        return arg1.intgr < arg2.intgr;
    case Kind::Float:
        return arg1.dble < arg2.dble;
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_nonatomic_unary.inc"
#undef SYMCXX_TYPE
        return instances[arg1.idx_pair.first] < instances[arg2.idx_pair.first];
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_nonatomic_binary.inc"
#undef SYMCXX_TYPE
        if (instances[arg1.idx_pair.first] == instances[arg2.idx_pair.first]){
            return instances[arg1.idx_pair.second] < instances[arg2.idx_pair.second];
        }
        return instances[arg1.idx_pair.first] < instances[arg2.idx_pair.first];
    default:
        break;
    }
    auto& c1 = args_stack[arg1.idx_pair.first];
    auto& c2 = args_stack[arg2.idx_pair.first];
    if (c1.size() != c2.size())
        return c1.size() < c2.size();
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = instances[c1[idx]];
        const auto& e2 = instances[c2[idx]];
        if (e1.kind != e2.kind)
            return e1.kind < e2.kind;
        if (e1 != e2)
            return e1 < e2;
    }
    return false;
}

inline bool symcxx::eq(const data_t arg1, const data_t arg2, const Kind kind,
                       const ArgStack_t& args_stack, const std::vector<Basic>& instances){
#if !defined(NDEBUG)
    std::cout << "eq(...)" << std::endl;
#endif
    switch(kind){
    case Kind::Symbol:
        return arg1.idx_pair.first == arg2.idx_pair.first;
    case Kind::Integer:
        return arg1.intgr == arg2.intgr;
    case Kind::Float:
        return arg1.dble == arg2.dble;
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_nonatomic_unary.inc"
#undef SYMCXX_TYPE
        return arg1.idx_pair.first == arg2.idx_pair.first;
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_nonatomic_binary.inc"
#undef SYMCXX_TYPE
        return ((arg1.idx_pair.first == arg2.idx_pair.first) &&
                (arg1.idx_pair.second == arg2.idx_pair.second));
    default:
        break;
    }
    auto& c1 = args_stack[arg1.idx_pair.first];
    auto& c2 = args_stack[arg2.idx_pair.first];
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
