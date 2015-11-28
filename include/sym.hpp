#ifndef SYM_HPP_K4U3Y2SQTBCBBL4FH37Z2ZEY2U
#define SYM_HPP_K4U3Y2SQTBCBBL4FH37Z2ZEY2U

#include <cmath>
#include <functional>
#include <vector>

// The following macros are defined for convenience, beware
#define SYM_BCAST(ptr) (*(BasicCont*)(ptr))
#define SYM_GET(item) (*item)

namespace sym{
    enum class Kind : int
    {
        Symbol,
        Integer,
        Float, // All before (incl.) Float are atomic
        Add,
        Mul,
        Sqrt,
        Pow,
        Sub,
        Div,
        ITE,
        Exp,
        Sin,
        Cos,
        Tan,
        Lt, // All after (incl.) Lt are relational
        Le,
        Eq,
        Ne,
        Ge,
        Gt
    };

    using idx_t = int;
    // using data_t = union { idx_t id; int i; double d; void * b;};
    union data_t { idx_t id; int i; double d; void * b;};

    struct Basic;
    // using BasicCont = std::vector<std::reference_wrapper<Basic> >;
    using BasicCont = std::vector<Basic*>;

    std::size_t calc_hash(BasicCont* data);



    bool lt(data_t arg1, data_t arg2, Kind kind);
    bool eq(data_t arg1, data_t arg2, Kind kind);

// #define ELEM0 (SYM_BCAST(this->data.b)[0].get())
// #define ELEM1 (SYM_BCAST(this->data.b)[1].get())
#define ELEM0 SYM_GET((SYM_BCAST(this->data.b)[0]))
#define ELEM1 SYM_GET((SYM_BCAST(this->data.b)[1]))
#define ELEM2 SYM_GET((SYM_BCAST(this->data.b)[2]))

    struct Basic {
        const std::size_t hash;
        const Kind kind;
        data_t data;

        bool is_atomic() const {
            return static_cast<int>(kind) <= static_cast<int>(Kind::Float);
        }
        double evalf(double inp[]) const {
            double result;
            switch(kind){
            case Kind::Symbol:
                result = inp[this->data.id]; break;
            case Kind::Float:
                result = this->data.d; break;
            case Kind::Integer:
                result = this->data.i; break;
            case Kind::Add:
                result = 0;
                for (const auto& v : SYM_BCAST(this->data.b))
                    result += SYM_GET(v).evalf(inp);
                break;
            case Kind::Mul:
                result = 1;
                for (const auto& v: SYM_BCAST(this->data.b))
                    result *= SYM_GET(v).evalf(inp);
                break;
            case Kind::Sqrt:
                result = std::sqrt(ELEM0.evalf(inp)); break;
            case Kind::Pow:
                result = std::pow(ELEM0.evalf(inp), ELEM1.evalf(inp)); break;
            case Kind::Sub:
                result = ELEM0.evalf(inp) - ELEM1.evalf(inp); break;
            case Kind::Div:
                result = ELEM0.evalf(inp) / ELEM1.evalf(inp); break;
            case Kind::ITE:
                result = (ELEM0.evalb(inp)) ? ELEM1.evalf(inp) : ELEM2.evalf(inp); break;
            case Kind::Exp:
                result = std::exp(ELEM0.evalf(inp)); break;
            case Kind::Sin:
                result = std::sin(ELEM0.evalf(inp)); break;
            case Kind::Cos:
                result = std::cos(ELEM0.evalf(inp)); break;
            case Kind::Tan:
                result = std::tan(ELEM0.evalf(inp)); break;
            default:
                throw std::runtime_error("Cannot run evalf for type");
            }
            return result;
        }
        bool evalb(double inp[]) const {
            switch(kind){
            case Kind::Lt:
                return ELEM0.evalf(inp) < ELEM1.evalf(inp);
            case Kind::Le:
                return ELEM0.evalf(inp) <= ELEM1.evalf(inp);
            case Kind::Eq:
                return ELEM0.evalf(inp) == ELEM1.evalf(inp);
            case Kind::Ne:
                return ELEM0.evalf(inp) != ELEM1.evalf(inp);
            case Kind::Ge:
                return ELEM0.evalf(inp) >= ELEM1.evalf(inp);
            case Kind::Gt:
                return ELEM0.evalf(inp) > ELEM1.evalf(inp);
            default:
                throw std::runtime_error("Cannot run evalb for type");
            }
        }
        Basic(std::size_t hash, Kind kind) : hash(hash), kind(kind) {}
        // Basic(Basic const&) = delete;
        // Basic& operator=(Basic const&) = delete;

        bool operator < (const Basic& other) const {
            if (kind != other.kind)
                return kind < other.kind;
            return lt(data, other.data, kind);
        }
        bool operator == (const Basic& other) const {
            if (kind != other.kind)
                return false;
            if (hash != other.hash)
                return false;
            return eq(data, other.data, kind);
        }
        // Use < and == to deduce rest of the operators:
        bool operator <= (const Basic& other) const {
            return (*this < other) or (*this == other);
        }
        bool operator != (const Basic& other) const {
            return not (*this == other);
        }
        bool operator >= (const Basic& other) const {
            return not (*this < other);
        }
        bool operator > (const Basic& other) const {
            return not (*this < other) and not (*this == other);
        }
    };

#undef ELEM2
#undef ELEM1
#undef ELEM0

    struct Composed : public Basic {
        Composed(BasicCont* args, Kind kind) : Basic(calc_hash(args), kind) {
            this->data.b = static_cast<void *>(args);
        }
    };

    struct Symbol : public Basic {
        Symbol(idx_t data) : Basic(std::hash<idx_t>()(data), Kind::Symbol) {
            this->data.id = data;
        }
    };
    struct Float : public Basic {
        Float(double data) : Basic(std::hash<double>()(data), Kind::Float) {
            this->data.d = data;
        }
    };
    struct Integer : public Basic {
        Integer(int data) : Basic(std::hash<int>()(data), Kind::Integer) {
            this->data.i = data;
        }
    };

    struct Unary : public Composed {
#if !defined(NDEBUG)
        Unary(BasicCont* args, Kind kind) : Composed(args, kind) {
            if ((*args).size() != 1)
                throw std::runtime_error("Unary takes one argument");
        }
#endif
    };
    struct Binary : public Composed {
#if !defined(NDEBUG)
        Binary(BasicCont* args, Kind kind) : Composed(args, kind) {
            if ((*args).size() != 1)
                throw std::runtime_error("Binary takes two arguments");
        }
#endif
    };
    struct Reduction : public Composed {
#if !defined(NDEBUG)
        Reduction(BasicCont* args, Kind kind) : Composed(args, kind) {
            if ((*args).size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
        }
#endif
    };

#define DECLARE(Name, Base) struct Name : public Base { Name(BasicCont* args) : Base(args, Kind::Name) {} }
    DECLARE(Add, Reduction);
    DECLARE(Mul, Reduction);
    DECLARE(Sqrt, Unary);
    DECLARE(Pow, Binary);
    DECLARE(Sub, Binary);
    DECLARE(Div, Binary);
    DECLARE(Exp, Unary);
    DECLARE(Sin, Unary);
    DECLARE(Cos, Unary);
    DECLARE(Tan, Unary);
    DECLARE(Lt, Binary);
    DECLARE(Le, Binary);
    DECLARE(Eq, Binary);
    DECLARE(Ne, Binary);
    DECLARE(Ge, Binary);
    DECLARE(Gt, Binary);
#undef DECLARE

    struct ITE : public Composed {
        ITE(BasicCont* args) : Composed(args, Kind::ITE)
#if !defined(NDEBUG)
        {
            if ((*args).size() != 3)
                throw std::runtime_error("ITE needs three arguments");
            if ((*args)[0]->kind < Kind::Lt)
                throw std::runtime_error("ITE arg0 needs to be a Relational");
            if ((*args)[1]->kind >= Kind::Lt)
                throw std::runtime_error("ITE arg1 cannot be a Relational");
            if ((*args)[2]->kind >= Kind::Lt)
                throw std::runtime_error("ITE arg2 cannot be a Relational");
        }
#else
            {}
#endif
    };

    struct NameSpace{
        // no reference counting, objects created in NameSpace live
        // there until the NameSpace instance is deleted.
        std::vector<Basic> instances;
        std::vector<BasicCont> args_stack;
        const idx_t nsymbs;
        NameSpace(int nsymbs) : nsymbs(nsymbs) {
            for (idx_t idx=0; idx < nsymbs; ++idx)
                instances.emplace_back(Symbol(idx));
        }
        BasicCont * reg(std::vector<idx_t> objs) {
            BasicCont args;
            for (const auto& idx : objs){
                args.push_back(&instances[idx]);
            }
            args_stack.push_back(args);
            return &args_stack.back();
        }
#define METH(Name, Constr) idx_t Name(std::vector<idx_t> objs){ \
            instances.emplace_back(Constr(reg(objs))); \
            return instances.size() - 1; \
        }
        METH(add, Add)
        METH(mul, Mul)
        METH(sqrt, Sqrt)
        METH(pow, Pow)
        METH(sub, Sub)
        METH(div, Div)
        METH(ite, ITE)
        METH(exp, Exp)
        METH(sin, Sin)
        METH(cos, Cos)
        METH(tan, Tan)
        METH(lt, Lt)
        METH(le, Le)
        METH(eq, Eq)
        METH(ne, Ne)
        METH(ge, Ge)
        METH(gt, Gt)
#undef METH

        double evalf(idx_t id, double inp[]){
            return instances[id].evalf(inp);
        }
        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };

};
#endif /* SYM_HPP_K4U3Y2SQTBCBBL4FH37Z2ZEY2U */
