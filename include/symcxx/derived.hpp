namespace symcxx {

    // Atomic:
    struct Symbol : public Basic {
        Symbol(const idx_t data, const NameSpace * const ns) : Basic(std::hash<idx_t>()(data), Kind::Symbol, data, ns) {}
    };
    struct MatProx : public Basic {
        MatProx(const idx_t data, const NameSpace * const ns) : Basic(std::hash<idx_t>()(data), Kind::MatProx, data, ns) {}
    };
    struct Float : public Basic {
        Float(const double data, const NameSpace * const ns) : Basic(std::hash<double>()(data), Kind::Float, data, ns) {}
    };
    struct Integer : public Basic {
        Integer(const intgr_t data,  const NameSpace * const ns) : Basic(std::hash<int64_t>()(data), Kind::Integer, data, ns) {}
    };

    // Non-atomic:
    struct Unary : public Basic {
        Unary(idx_t inst_idx, const Kind kind, const NameSpace * const ns) :
            Basic(calc_hash(inst_idx, kind, ns->instances), kind, inst_idx, ns) {}
    };
    struct Binary : public Basic {
        Binary(const idx_t inst_idx0, const idx_t inst_idx1, const Kind kind, const NameSpace * const ns) :
            Basic(calc_hash(inst_idx0, inst_idx1, kind, ns->instances), kind, idx_pair_t({inst_idx0, inst_idx1}), ns) {}
    };

    struct Composed : public Basic {
        Composed(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Basic(calc_hash(args_idx, kind, ns->instances, ns->args_stack), kind, args_idx, ns) {}
    };

    struct Reduction : public Composed {
        Reduction(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns)
        {
#if !defined(NDEBUG)
            if (ns->args_stack[args_idx].size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
#endif
        }
    };

    struct Ternary : public Composed {
        Ternary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns)
        {
#if !defined(NDEBUG)
            if (ns->args_stack[args_idx].size() != 3)
                throw std::runtime_error("ITE needs three arguments");
            if (ns->instances[ns->args_stack[args_idx][0]].kind < Kind::Lt)
                throw std::runtime_error("ITE arg0 needs to be a Relational");
            if (ns->instances[ns->args_stack[args_idx][1]].kind >= Kind::Lt)
                throw std::runtime_error("ITE arg1 cannot be a Relational");
            if (ns->instances[ns->args_stack[args_idx][2]].kind >= Kind::Lt)
                throw std::runtime_error("ITE arg2 cannot be a Relational");
#endif
        }
    };

#define SYMCXX_TYPE(Cls, Parent, meth)                                  \
    struct Cls : public Parent {                                        \
        Cls(idx_t idx,  const NameSpace * const ns) :                   \
            Parent(idx, Kind::Cls, ns) {}                               \
    };
#include "symcxx/types_nonatomic_args_stack.inc"
#include "symcxx/types_nonatomic_unary.inc"
#undef SYMCXX_TYPE

#define SYMCXX_TYPE(Cls, Parent, meth)                                  \
    struct Cls : public Parent {                                        \
        Cls(idx_t idx0, idx_t idx1, const NameSpace * const ns) :       \
            Parent(idx0, idx1, Kind::Cls, ns) {}                        \
    };
#include "symcxx/types_nonatomic_binary.inc"
#undef SYMCXX_TYPE

}
