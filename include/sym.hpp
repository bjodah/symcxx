#pragma once

#include <cmath>
#include <functional>
#include <vector>
#if !defined(NDEBUG)
#include <iostream>
#endif

// The following macros are defined for convenience, beware
#define SYM_BCAST(ptr) (*static_cast<const BasicCont *>(ptr))

namespace sym{
    using hash_t = int; // std::size_t;
    using idx_t = std::size_t;
    union data_t {
        idx_t id;
        int i;
        double d;
        const void * const b;
        data_t(idx_t id) : id(id) {}
        data_t(int i) : i(i) {}
        data_t(double d) : d(d) {}
        data_t(const void * const b) : b(b) {}
    };

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
    static_assert(static_cast<int>(Kind::Gt) < (sizeof(hash_t)*8-1),
                  "Too many Kinds for current hash.");

    struct Basic;
    using BasicCont = std::vector<Basic *>;

    hash_t calc_hash(const BasicCont * const data, const Kind kind);
    bool lt(const data_t arg1, const data_t arg2, const Kind kind);
    bool eq(const data_t arg1, const data_t arg2, const Kind kind);

// #define ELEM0 (SYM_BCAST(this->data.b)[0].get())
// #define ELEM1 (SYM_BCAST(this->data.b)[1].get())
#define ELEM(id) SYM_BCAST(this->data.b)[id]

    struct Basic {
        const hash_t hash;
        const Kind kind;
        const data_t data;
        Basic(const hash_t hash, const Kind kind, const data_t data) :
            hash(hash), kind(kind), data(data) {}

//         Basic(hash_t hash, Kind kind) : hash(hash), kind(kind) {
// #if !defined(NDEBUG)
//             std::cout << "Basic(hash="<< hash <<",kind="<< static_cast<int>(kind) <<")" <<  std::endl;
// #endif
//         }
        // Basic(Basic const&) = delete;
        // Basic& operator=(Basic const&) = delete;

        bool is_atomic() const {
            return static_cast<int>(kind) <= static_cast<int>(Kind::Float);
        }
        double evalf(const double inp[]) const {
            double result;
#if !defined(NDEBUG)
            std::cout << "evalf, kind=" << static_cast<int>(kind) << ", this=" << this << ", this->data.b=" << this->data.b << std::endl;
            if (static_cast<int>(kind) > static_cast<int>(Kind::Float))
                std::cout << "   ELEM(0)->hash=" << ELEM(0)->hash << std::endl;
#endif
            switch(kind){
            case Kind::Symbol:
#if !defined(NDEBUG)
                std::cout << "inp[this("<< this<<")->data.id=" << this->data.id << "] = " <<
                          inp[this->data.id] << std::endl;
#endif
                result = inp[this->data.id]; break;
            case Kind::Float:
                result = this->data.d; break;
            case Kind::Integer:
                result = this->data.i; break;
            case Kind::Add:
                result = 0;
                for (const auto basic_ptr : SYM_BCAST(this->data.b))
                    result += basic_ptr->evalf(inp);
                break;
            case Kind::Mul:
                result = 1;
                for (const auto basic_ptr: SYM_BCAST(this->data.b))
                    result *= basic_ptr->evalf(inp);
                break;
            case Kind::Sqrt:
                result = std::sqrt(ELEM(0)->evalf(inp)); break;
            case Kind::Pow:
                result = std::pow(ELEM(0)->evalf(inp), ELEM(1)->evalf(inp)); break;
            case Kind::Sub:
                result = ELEM(0)->evalf(inp) - ELEM(1)->evalf(inp); break;
            case Kind::Div:
                result = ELEM(0)->evalf(inp) / ELEM(1)->evalf(inp); break;
            case Kind::ITE:
                result = (ELEM(0)->evalb(inp)) ? ELEM(1)->evalf(inp) : ELEM(2)->evalf(inp); break;
            case Kind::Exp:
                result = std::exp(ELEM(0)->evalf(inp)); break;
            case Kind::Sin:
                result = std::sin(ELEM(0)->evalf(inp)); break;
            case Kind::Cos:
                result = std::cos(ELEM(0)->evalf(inp)); break;
            case Kind::Tan:
                result = std::tan(ELEM(0)->evalf(inp)); break;
            default:
                throw std::runtime_error("Cannot run evalf for type.");
            }
            return result;
        }
        bool evalb(const double inp[]) const {
            switch(kind){
            case Kind::Lt:
                return ELEM(0)->evalf(inp) < ELEM(1)->evalf(inp);
            case Kind::Le:
                return ELEM(0)->evalf(inp) <= ELEM(1)->evalf(inp);
            case Kind::Eq:
                return ELEM(0)->evalf(inp) == ELEM(1)->evalf(inp);
            case Kind::Ne:
                return ELEM(0)->evalf(inp) != ELEM(1)->evalf(inp);
            case Kind::Ge:
                return ELEM(0)->evalf(inp) >= ELEM(1)->evalf(inp);
            case Kind::Gt:
                return ELEM(0)->evalf(inp) > ELEM(1)->evalf(inp);
            default:
                throw std::runtime_error("Cannot run evalb for type");
            }
        }

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

#undef ELEM

    struct Composed : public Basic {
        Composed(const BasicCont * const args, const Kind kind) : Basic(calc_hash(args, kind), kind, static_cast<const void * const>(args)) {}
    };

    struct Symbol : public Basic {
        Symbol(const idx_t data) : Basic(std::hash<idx_t>()(data), Kind::Symbol, data) {}
    };
    struct Float : public Basic {
        Float(const double data) : Basic(std::hash<double>()(data), Kind::Float, data) {}
    };
    struct Integer : public Basic {
        Integer(const int data) : Basic(std::hash<int>()(data), Kind::Integer, data) {}
    };

    struct Unary : public Composed {
#if !defined(NDEBUG)
        Unary(const BasicCont * const args, const Kind kind) : Composed(args, kind) {
            if ((*args).size() != 1)
                throw std::runtime_error("Unary takes one argument");
        }
#endif
    };
    struct Binary : public Composed {
#if !defined(NDEBUG)
        Binary(const BasicCont * const args, const Kind kind) : Composed(args, kind) {
            if ((*args).size() != 1)
                throw std::runtime_error("Binary takes two arguments");
        }
#endif
    };
    struct Reduction : public Composed {
#if !defined(NDEBUG)
        Reduction(const BasicCont * const args, const Kind kind) : Composed(args, kind) {
            if ((*args).size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
        }
#endif
    };

#define DECLARE(Name, Base) struct Name : public Base { Name(const BasicCont * const args) : Base(args, Kind::Name) {} }
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
        ITE(const BasicCont * const args) : Composed(args, Kind::ITE)
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
        NameSpace(idx_t nsymbs) : nsymbs(nsymbs) {
            for (idx_t idx=0; idx < nsymbs; ++idx)
                instances.emplace_back(Symbol(idx));
        }
        BasicCont * reg(const std::vector<idx_t>& objs) {
            BasicCont args;
            for (const auto& idx : objs){
                args.push_back(&instances[idx]);
            }
            args_stack.push_back(args);
            return &args_stack.back();
        }
#define METH(Name, Constr) idx_t Name(const std::vector<idx_t>& objs){ \
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

        double evalf(const idx_t id, const double inp[]){
            std::cout << "ns.evalf(id: " << id << ", inp: ["<< inp[0]<<" ,...])"<< std::endl;
            return instances[id].evalf(inp);
        }
        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };

};
