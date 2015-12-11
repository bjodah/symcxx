#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct NameSpace{
        // no reference counting, objects created in NameSpace live
        // there until the NameSpace instance is deleted.
        std::vector<Basic> instances;
        std::vector<std::vector<idx_t> > args_stack;
        const idx_t n_pre_symbs = 3;
        idx_t n_symbs;
        // idx_t __padding {0};
        const static idx_t n_pre_intgrs = 7;
        const static idx_t n_special = 6; // see below:
        const static idx_t pi_id = n_pre_intgrs; // atan(1)*4
        const static idx_t neg_pi_id = n_pre_intgrs+1; // -atan(1)*4
        const static idx_t e_id = n_pre_intgrs+2; // exp(1)
        const static idx_t neg_e_id = n_pre_intgrs+3; // -exp(1)
        const static idx_t ln2_id = n_pre_intgrs+4; // log(2)
        const static idx_t ln10_id = n_pre_intgrs+5; // log(10)


        NameSpace(idx_t=0);
        // idx_t idx(const Basic*) const;
        idx_t reg_args(const std::vector<idx_t>&);
        bool has(const Basic&, idx_t*) const;
        bool is_zero(const idx_t) const;
        bool is_one(const idx_t) const;
        bool apparently_negative(const idx_t) const;
        idx_t make_symbol(idx_t i);
        idx_t make_symbol();
        std::vector<idx_t> make_symbols(idx_t n);
        idx_t make_integer(int i);
        idx_t make_float(double f);
        idx_t make_nan();
        std::string print_ast(const idx_t, const std::vector<std::string>&) const;

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
        idx_t create(const Kind, const idx_t);
        idx_t create(const Kind, const idx_t, const idx_t);
        double evalf(const idx_t id, const double inp[]) const;
        bool evalb(const idx_t id, const double inp[]) const;
        idx_t diff(const idx_t inst_id, const idx_t wrt_id);

        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;

    private:
        std::vector<idx_t> collect(const Kind, const std::vector<idx_t>&);
        std::vector<idx_t> merge(const Kind, const std::vector<idx_t>&) const;
        std::vector<idx_t> sort_args(const std::vector<idx_t>&) const;
        std::vector<idx_t> merge_drop_sort_collect(const std::vector<idx_t>&, const Kind,
                                                   const std::vector<idx_t>&, const Kind);
    };

}
static_assert(symcxx::NameSpace::n_pre_intgrs >= 7, "We need 0, 1, 2, 3 to be zero, one, two and three"); // see is_zero, is_one
