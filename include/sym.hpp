#pragma once

#include <cmath>
#include <functional>
#include <vector>
#if !defined(NDEBUG)
#include <iostream>
#endif

// The following macros are defined for convenience, beware
#define SYM_BCAST(ptr) (*static_cast<const BasicPtrVec *>(ptr))

namespace sym{
    using hash_t = int; //std::size_t;
    using idx_t = std::size_t;
    union data_t {
        const idx_t id;
        const int i;
        const double d;
        const void * const b;
        data_t(const idx_t id) : id(id) {}
        data_t(const int i) : i(i) {}
        data_t(const double d) : d(d) {}
        data_t(const void * const b) : b(b) {}
    };

    enum class Kind : int
    {
        Symbol,
        Integer,
        Float, // All before (incl.) Float are atomic
        Add,
        Mul,
        Sub,
        Div,
        Cos,
        Sin,
        Tan,
        Acos,
        Asin,
        Atan,
        Atan2,
        Cosh,
        Sinh,
        Tanh,
        Acosh,
        Asinh,
        Atanh,
        Exp,
        Log,
        Log10,
        Exp2,
        Expm1,
        Log1p,
        Log2,
        Logb,
        Pow,
        Sqrt,
        Cbrt,
        Hypot,
        Erf,
        Erfc,
        Tgamma,
        Lgamma,
        ITE,
        Lt, // All after (incl.) Lt are relational
        Le,
        Eq,
        Ne,
        Ge,
        Gt
    };

    struct Basic;
    struct NameSpace;
    using BasicPtrVec = std::vector<Basic *>;

    hash_t calc_hash(const BasicPtrVec * const data, const Kind kind);
    bool lt(const data_t arg1, const data_t arg2, const Kind kind);
    bool eq(const data_t arg1, const data_t arg2, const Kind kind);

    struct Basic {
        const data_t data;
        const Kind kind;
        const hash_t hash;
        Basic(const hash_t hash, const Kind kind, const data_t data) :
            data(data), kind(kind), hash(hash) {}
        // Basic(Basic const&) = delete;
        // Basic& operator=(Basic const&) = delete;

        const BasicPtrVec& args() const {
            return *static_cast<const BasicPtrVec *>(data.b);
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
        bool is_atomic() const {
            return static_cast<int>(kind) <= static_cast<int>(Kind::Float);
        }
        double evalf(const double inp[]) const {
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
                for (const auto basic_ptr : this->args())
                    result += basic_ptr->evalf(inp);
                break;
            case Kind::Mul:
                result = 1;
                for (const auto basic_ptr: this->args())
                    result *= basic_ptr->evalf(inp);
                break;
            case Kind::Sub:
                result = args()[0]->evalf(inp) - args()[1]->evalf(inp); break;
            case Kind::Div:
                result = args()[0]->evalf(inp) / args()[1]->evalf(inp); break;
            case Kind::Cos:
                result = std::cos(args()[0]->evalf(inp)); break;
            case Kind::Sin:
                result = std::sin(args()[0]->evalf(inp)); break;
            case Kind::Tan:
                result = std::tan(args()[0]->evalf(inp)); break;
            case Kind::Acos:
                result = std::acos(args()[0]->evalf(inp)); break;
            case Kind::Asin:
                result = std::asin(args()[0]->evalf(inp)); break;
            case Kind::Atan:
                result = std::atan(args()[0]->evalf(inp)); break;
            case Kind::Atan2:
                result = std::atan2(args()[0]->evalf(inp), args()[1]->evalf(inp)); break;
            case Kind::Cosh:
                result = std::cosh(args()[0]->evalf(inp)); break;
            case Kind::Sinh:
                result = std::sinh(args()[0]->evalf(inp)); break;
            case Kind::Tanh:
                result = std::tanh(args()[0]->evalf(inp)); break;
            case Kind::Acosh:
                result = std::acosh(args()[0]->evalf(inp)); break;
            case Kind::Asinh:
                result = std::asinh(args()[0]->evalf(inp)); break;
            case Kind::Atanh:
                result = std::atanh(args()[0]->evalf(inp)); break;
            case Kind::Exp:
                result = std::exp(args()[0]->evalf(inp)); break;
            case Kind::Log:
                result = std::log(args()[0]->evalf(inp)); break;
            case Kind::Log10:
                result = std::exp(args()[0]->evalf(inp)); break;
            case Kind::Exp2:
                result = std::exp2(args()[0]->evalf(inp)); break;
            case Kind::Expm1:
                result = std::expm1(args()[0]->evalf(inp)); break;
            case Kind::Log1p:
                result = std::log1p(args()[0]->evalf(inp)); break;
            case Kind::Log2:
                result = std::log2(args()[0]->evalf(inp)); break;
            case Kind::Logb:
                result = std::logb(args()[0]->evalf(inp)); break;
            case Kind::Pow:
                result = std::pow(args()[0]->evalf(inp), args()[1]->evalf(inp)); break;
            case Kind::Sqrt:
                result = std::sqrt(args()[0]->evalf(inp)); break;
            case Kind::Cbrt:
                result = std::cbrt(args()[0]->evalf(inp)); break;
            case Kind::Hypot:
                result = std::hypot(args()[0]->evalf(inp), args()[1]->evalf(inp)); break;
            case Kind::Erf:
                result = std::erf(args()[0]->evalf(inp)); break;
            case Kind::Erfc:
                result = std::erfc(args()[0]->evalf(inp)); break;
            case Kind::Tgamma:
                result = std::tgamma(args()[0]->evalf(inp)); break;
            case Kind::Lgamma:
                result = std::lgamma(args()[0]->evalf(inp)); break;
            case Kind::ITE:
                result = (args()[0]->evalb(inp)) ? args()[1]->evalf(inp) : args()[2]->evalf(inp); break;
            default:
                throw std::runtime_error("Cannot run evalf for type.");
            }
            return result;
        }
        bool evalb(const double inp[]) const {
            switch(kind){
            case Kind::Lt:
                return (args()[0]->evalf(inp))  < (args()[1]->evalf(inp));
            case Kind::Le:
                return (args()[0]->evalf(inp)) <= (args()[1]->evalf(inp));
            case Kind::Eq:
                return (args()[0]->evalf(inp)) == (args()[1]->evalf(inp));
            case Kind::Ne:
                return (args()[0]->evalf(inp)) != (args()[1]->evalf(inp));
            case Kind::Ge:
                return (args()[0]->evalf(inp)) >= (args()[1]->evalf(inp));
            case Kind::Gt:
                return (args()[0]->evalf(inp))  > (args()[1]->evalf(inp));
            default:
                throw std::runtime_error("Cannot run evalb for type");
            }
        }
    };

    struct Composed : public Basic {
        Composed(const BasicPtrVec * const args, const Kind kind) :
            Basic(calc_hash(args, kind), kind, static_cast<const void * const>(args)) {}
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
        Unary(const BasicPtrVec * const args, const Kind kind) : Composed(args, kind) {
            if ((*args).size() != 1)
                throw std::runtime_error("Unary takes one argument");
        }
#endif
    };
    struct Binary : public Composed {
#if !defined(NDEBUG)
        Binary(const BasicPtrVec * const args, const Kind kind) : Composed(args, kind) {
            if ((*args).size() != 2)
                throw std::runtime_error("Binary takes two arguments");
        }
#endif
    };
    struct Reduction : public Composed {
#if !defined(NDEBUG)
        Reduction(const BasicPtrVec * const args, const Kind kind) : Composed(args, kind) {
            if ((*args).size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
        }
#endif
    };

#define DECLARE(Name, Base) struct Name : public Base { Name(const BasicPtrVec * const args) : Base(args, Kind::Name) {} }
    DECLARE(Add, Reduction);
    DECLARE(Mul, Reduction);
    DECLARE(Sub, Binary);
    DECLARE(Div, Binary);
    DECLARE(Cos, Unary);
    DECLARE(Sin, Unary);
    DECLARE(Tan, Unary);
    DECLARE(Acos, Unary);
    DECLARE(Asin, Unary);
    DECLARE(Atan, Unary);
    DECLARE(Atan2, Binary);
    DECLARE(Cosh, Unary);
    DECLARE(Sinh, Unary);
    DECLARE(Tanh, Unary);
    DECLARE(Acosh, Unary);
    DECLARE(Asinh, Unary);
    DECLARE(Atanh, Unary);
    DECLARE(Exp, Unary);
    DECLARE(Log, Unary);
    DECLARE(Log10, Unary);
    DECLARE(Exp2, Unary);
    DECLARE(Expm1, Unary);
    DECLARE(Log1p, Unary);
    DECLARE(Log2, Unary);
    DECLARE(Logb, Unary);
    DECLARE(Pow, Binary);
    DECLARE(Sqrt, Unary);
    DECLARE(Cbrt, Unary);
    DECLARE(Hypot, Binary);
    DECLARE(Erf, Unary);
    DECLARE(Erfc, Unary);
    DECLARE(Tgamma, Unary);
    DECLARE(Lgamma, Unary);
    DECLARE(Lt, Binary);
    DECLARE(Le, Binary);
    DECLARE(Eq, Binary);
    DECLARE(Ne, Binary);
    DECLARE(Ge, Binary);
    DECLARE(Gt, Binary);
#undef DECLARE

    struct ITE : public Composed {
        ITE(const BasicPtrVec * const args) : Composed(args, Kind::ITE)
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
        std::vector<BasicPtrVec> args_stack;
        const idx_t nsymbs;
        NameSpace(idx_t nsymbs) : nsymbs(nsymbs) {
            instances.reserve(1024*1024); // <-- Very, very bad
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
        BasicPtrVec * reg(const std::vector<idx_t>& objs) {
            BasicPtrVec args;
            for (const auto& idx : objs){
                args.push_back(&instances[idx]);
            }
            args_stack.push_back(args);
            return &args_stack.back();
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
            instances.push_back(Constr(reg(objs))); \
            return instances.size() - 1; \
        }
        METH(add, Add)
        METH(mul, Mul)
        METH(sub, Sub)
        METH(div, Div)
        METH(cos, Cos)
        METH(sin, Sin)
        METH(tan, Tan)
        METH(acos, Acos)
        METH(asin, Asin)
        METH(atan, Atan)
        METH(atan2, Atan2)
        METH(cosh, Cosh)
        METH(sinh, Sinh)
        METH(tanh, Tanh)
        METH(acosh, Acosh)
        METH(asinh, Asinh)
        METH(atanh, Atanh)
        METH(exp, Exp)
        METH(log, Log)
        METH(log10, Log10)
        METH(exp2, Exp2)
        METH(expm1, Expm1)
        METH(log1p, Log1p)
        METH(log2, Log2)
        METH(logb, Logb)
        METH(pow, Pow)
        METH(sqrt, Sqrt)
        METH(cbrt, Cbrt)
        METH(hypot, Hypot)
        METH(erf, Erf)
        METH(erfc, Erfc)
        METH(tgamma, Tgamma)
        METH(lgamma, Lgamma)
        METH(ite, ITE)
        METH(lt, Lt)
        METH(le, Le)
        METH(eq, Eq)
        METH(ne, Ne)
        METH(ge, Ge)
        METH(gt, Gt)
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

        idx_t diff(const idx_t inst_id, const idx_t wrt_id) {
            const Basic& inst = instances[inst_id];
            const Basic& wrt = instances[wrt_id];
            return diff(inst, wrt);
        }
        idx_t diff(const Basic& inst, const Basic& wrt);
        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };

}

inline sym::hash_t sym::calc_hash(const BasicPtrVec * const data, const Kind kind) {
    const sym::hash_t most_significant = static_cast<sym::hash_t>(1) << (8*sizeof(sym::hash_t) - 1);
    sym::hash_t result = most_significant >> static_cast<int>(kind);
    result = result ^ most_significant >> static_cast<int>(kind)/2;
    for (const auto basic_ptr : *data)
        result = result ^ basic_ptr->hash;
    return result;
}

inline bool sym::lt(const data_t arg1, const data_t arg2, const Kind kind){
    switch(kind){
    case Kind::Symbol:
        return arg1.id < arg2.id;
    case Kind::Integer:
        return arg1.i < arg2.i;
    case Kind::Float:
        return arg1.d < arg2.d;
    default:
        break;
    }
    auto& c1 = SYM_BCAST(arg1.b);
    auto& c2 = SYM_BCAST(arg2.b);
    if (c1.size() != c2.size())
        return c1.size() < c2.size();
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = *c1[idx];
        const auto& e2 = *c2[idx];
        if (e1.kind != e2.kind)
            return e1.kind < e2.kind;
        if (!(e1 == e2))
            return e1 < e2;
    }
    return false;
}

inline bool sym::eq(const data_t arg1, const data_t arg2, const Kind kind){
    switch(kind){
    case Kind::Symbol:
        return arg1.id == arg2.id;
    case Kind::Integer:
        return arg1.i == arg2.i;
    case Kind::Float:
        return arg1.d == arg2.d;
    default:
        break;
    }
    auto& c1 = SYM_BCAST(arg1.b);
    auto& c2 = SYM_BCAST(arg2.b);
    if (c1.size() != c2.size())
        return false;
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        const auto& e1 = *c1[idx];
        const auto& e2 = *c2[idx];
        if (!(e1 == e2))
            return false;
    }
    return true;
}
