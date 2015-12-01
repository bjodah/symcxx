#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct NameSpace{
        // no reference counting, objects created in NameSpace live
        // there until the NameSpace instance is deleted.
        std::vector<Basic> instances;
        std::vector<std::vector<idx_t> > args_stack;
        const idx_t nsymbs;
        NameSpace(idx_t);
        idx_t idx(const Basic*) const;
        idx_t reg(const std::vector<idx_t>&);
        bool has(const Basic&, idx_t*) const;
        idx_t make_integer(int i);
        idx_t make_float(double f);

#define METH(Name, Constr) idx_t Name(const std::vector<idx_t>&);
#define SYMCXX_TYPE(Cls, Parent, meth) METH(meth, Cls)
#include "symcxx/types_composed.inc"
#undef SYMCXX_TYPE
#undef METH


        idx_t create(const Kind, const std::vector<idx_t>&);
        double evalf(const idx_t id, const double inp[]) const;
        idx_t diff(const idx_t inst_id, const idx_t wrt_id);

        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };
}
