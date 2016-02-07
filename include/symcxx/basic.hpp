#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct Basic {
        const data_t data;
        const Kind kind;
        const hash_t hash;
        const NameSpace * const ns;
        Basic(const hash_t hash, const Kind kind, const data_t data, const NameSpace * const ns) :
            data(data), kind(kind), hash(hash), ns(ns) {}
        // Basic(Basic const&) = delete;
        // Basic& operator=(Basic const&) = delete;

        const std::vector<idx_t>& args_from_stack() const;
        bool operator < (const Basic& other) const;
        bool operator == (const Basic& other) const;

        // Operators < and == are used to deduce rest of the operators:
        bool operator <= (const Basic& other) const;
        bool operator != (const Basic& other) const;
        bool operator >= (const Basic& other) const;
        bool operator > (const Basic& other) const;

        std::string print(const std::vector<std::string>&) const;

        bool is_atomic() const;
        double evalf(const double inp[]) const;
        bool evalb(const double inp[]) const;
    };

    static_assert((sizeof(Basic) == 8+4+4+8) ||
                  (sizeof(Basic) == 8+4+4+4),
                  "24-bytes (64-bit system) or 20-bytes (32-bit system)");
    // 8, 2, 2, 4 ? by using data_t/short/short/x32 ?
    // that would give twice as many objects per L1 cache
    // line. (4 vs. 2)
}
