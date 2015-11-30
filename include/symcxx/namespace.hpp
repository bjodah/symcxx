#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct NameSpace{
        // no reference counting, objects created in NameSpace live
        // there until the NameSpace instance is deleted.
        std::vector<Basic> instances;
        std::vector<std::vector<idx_t> > args_stack;
        const idx_t nsymbs;
        NameSpace(idx_t nsymbs) : nsymbs(nsymbs) {
            for (idx_t idx=0; idx < nsymbs; ++idx)
                instances.push_back(Symbol(idx));
        }
        idx_t idx(const Basic* basic_ptr) const {
            const auto begin = &instances[0];
            const auto end = begin + instances.capacity();
            if ((basic_ptr < begin) || (basic_ptr > end))
                throw std::runtime_error("Instance not in namespace.");
            return (basic_ptr - begin);
        }
        idx_t reg(const std::vector<idx_t>& objs) {
            args_stack.push_back(objs);
            return args_stack.size() - 1;
        }
        bool has(const Basic& looking_for, idx_t * idx) const {
            idx_t idx_ = 0;
            for (const auto& inst : instances){
                if (looking_for == inst){
                    *idx = idx_;
                    return true;
                }
                ++idx_;
            }
            return false;
        }
        idx_t make_integer(int i){
            std::cout << "make_integer(" << i << ")" << std::endl;
            const auto instance = Integer(i);
            idx_t idx;
            if (has(instance, &idx))
                return idx;
            instances.push_back(instance);
            return instances.size() - 1;
        }
        idx_t make_float(double f){
            const auto instance = Float(f);
            idx_t idx;
            if (has(instance, &idx))
                return idx;
            instances.push_back(instance);
            return instances.size() - 1;
        }
#define METH(Name, Constr) idx_t Name(const std::vector<idx_t>& objs){ \
            instances.push_back(Constr(reg(objs), this)); \
            return instances.size() - 1; \
        }
#define SYMCXX_TYPE(Cls, Parent, meth) METH(meth, Cls)
#include "symcxx/types_composed.inc"
#undef SYMCXX_TYPE
#undef METH

        idx_t create(const Kind kind, const std::vector<idx_t>& objs){
            switch(kind){
            case Kind::Add:
                return add(objs);
            default:
                throw std::runtime_error("create does not support kind.");
            }
        }

        double evalf(const idx_t id, const double inp[]) const {
            return instances[id].evalf(inp);
        }

        idx_t diff(const idx_t inst_id, const idx_t wrt_id);

        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };
}
