namespace symcxx {

    // Atomic:
    struct Symbol : public Basic {
        Symbol(const idx_t data, const NameSpace * const ns) : Basic(std::hash<idx_t>()(data), Kind::Symbol, data, ns) {}
    };
    struct Float : public Basic {
        Float(const double data, const NameSpace * const ns) : Basic(std::hash<double>()(data), Kind::Float, data, ns) {}
    };
    struct Integer : public Basic {
        Integer(const int data,  const NameSpace * const ns) : Basic(std::hash<int>()(data), Kind::Integer, data, ns) {}
    };

    // Non-atomic:
    struct Composed : public Basic {
        Composed(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Basic(calc_hash(args_idx, kind, ns->args_stack, ns->instances), kind, args_idx, ns) {}
    };

    struct Unary : public Composed {
#if !defined(NDEBUG)
        Unary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns) {
            if (ns->args_stack[args_idx].size() != 1)
                throw std::runtime_error("Unary takes one argument");
        }
#endif
    };
    struct Binary : public Composed {
#if !defined(NDEBUG)
        Binary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns) {
            if (ns->args_stack[args_idx].size() != 2)
                throw std::runtime_error("Binary takes two arguments");
        }
#endif
    };
    struct Reduction : public Composed {
#if !defined(NDEBUG)
        Reduction(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns) {
            if (ns->args_stack[args_idx].size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
        }
#endif
    };

    struct Ternary : public Composed {
#if !defined(NDEBUG)
        Ternary(idx_t args_idx, const Kind kind, const NameSpace * const ns) :
            Composed(args_idx, kind, ns)
        {
            if (ns->args_stack[args_idx].size() != 3)
                throw std::runtime_error("ITE needs three arguments");
            if (ns->instances[ns->args_stack[args_idx][0]].kind < Kind::Lt)
                throw std::runtime_error("ITE arg0 needs to be a Relational");
            if (ns->instances[ns->args_stack[args_idx][1]].kind >= Kind::Lt)
                throw std::runtime_error("ITE arg1 cannot be a Relational");
            if (ns->instances[ns->args_stack[args_idx][2]].kind >= Kind::Lt)
                throw std::runtime_error("ITE arg2 cannot be a Relational");
        }
#endif
    };

#define S(x) #x
#define SX(x) S(x)

#define DECLARE(Name, Base) struct Name : public Base {     \
        Name(idx_t args_idx,  const NameSpace * const ns) : \
            Base(args_idx, Kind::Name, ns) {                \
            std::cout << "constructing " SX(Name) " with args_idx=" << args_idx << std::endl; \
        }                                                   \
    };
#define SYMCXX_TYPE(Cls, Parent, meth) DECLARE(Cls, Parent)
#include "symcxx/types_composed.inc"
#undef SYMCXX_TYPE
#undef DECLARE
}
