#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct NameSpace{
        // no reference counting, objects created in NameSpace live
        // there until the NameSpace instance is deleted.
        std::vector<Basic> instances;
        std::vector<std::vector<idx_t> > args_stack;
        const idx_t nsymbs;
        const idx_t padding___ = {0};
        NameSpace(idx_t);
        idx_t idx(const Basic*) const;
        idx_t reg_args(const std::vector<idx_t>&);
        bool has(const Basic&, idx_t*) const;
        idx_t make_integer(int i);
        idx_t make_float(double f);

#define SYMCXX_TYPE(Cls, Parent, meth) idx_t meth(const std::vector<idx_t>&);
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE

#define SYMCXX_TYPE(Cls, Parent, meth) idx_t meth(const idx_t);
#include "symcxx/types_nonatomic_unary.inc"
#undef SYMCXX_TYPE

#define SYMCXX_TYPE(Cls, Parent, meth) idx_t meth(const idx_t, const idx_t);
#include "symcxx/types_nonatomic_binary.inc"
#undef SYMCXX_TYPE

        idx_t create(const Kind, const std::vector<idx_t>&);
        double evalf(const idx_t id, const double inp[]) const;
        idx_t diff(const idx_t inst_id, const idx_t wrt_id);

        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };
}
