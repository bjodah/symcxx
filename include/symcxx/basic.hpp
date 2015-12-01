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

        const std::vector<idx_t>& args() const;
        bool operator < (const Basic& other) const;
        bool operator == (const Basic& other) const;

        // Operators < and == are used to deduce rest of the operators:
        bool operator <= (const Basic& other) const;
        bool operator != (const Basic& other) const;
        bool operator >= (const Basic& other) const;
        bool operator > (const Basic& other) const;

        bool is_atomic() const;
        double evalf(const double inp[]) const;
        bool evalb(const double inp[]) const;
    };
}
