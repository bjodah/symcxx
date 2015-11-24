#include <cmath>
#include <functional>
#include <vector>

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
        Undefined
    };

    using idx_t = int;
    using data_t = union { idx_t id; int i; double d; void * b;};

    class Basic;
    using BasicCont = std::vector<std::reference_wrapper<Basic> >;

    std::size_t calc_hash(BasicCont data);


// #define BCAST(ptr) static_cast<BasicCont>(ptr)
#define BCAST(ptr) (*(BasicCont*)(ptr))

    bool lt(data_t arg1, data_t arg2, Kind kind);
    bool eq(data_t arg1, data_t arg2, Kind kind);

#define ELEM0 (BCAST(this->data.b)[0].get())
#define ELEM1 (BCAST(this->data.b)[1].get())

    struct Basic {
        const Kind kind = Kind::Undefined;
        data_t data;
        const std::size_t hash;

        // pure virtual member functions (must be overloaded)
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
                for (const auto& v : BCAST(this->data.b))
                    result += v.get().evalf(inp);
                break;
            case Kind::Mul:
                result = 1;
                for (const auto& v: BCAST(this->data.b))
                    result *= v.get().evalf(inp);
                break;
            case Kind::Sqrt:
                result = std::sqrt(ELEM0.evalf(inp)); break;
            case Kind::Pow:
                result = std::pow(ELEM0.evalf(inp), ELEM1.evalf(inp)); break;
            case Kind::Sub:
                result = ELEM0.evalf(inp) - ELEM1.evalf(inp); break;
            case Kind::Div:
                result = ELEM0.evalf(inp) / ELEM1.evalf(inp); break;
            default:
                throw std::runtime_error("Cannot run evalf for type");
            }
            return result;
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
    };
#undef ELEM1
#undef ELEM0

    struct Composed : public Basic {
        data_t data;
        Composed(BasicCont args, Kind kind) : Basic(calc_hash(args), kind) {
            data.b = static_cast<void *>(&args);
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
        Unary(BasicCont args, Kind kind) : Composed(args, kind) {
            if (args.size() != 1)
                throw std::runtime_error("Unary takes one argument");
        }
#endif
    };
    struct Binary : public Composed {
#if !defined(NDEBUG)
        Binary(BasicCont args, Kind kind) : Composed(args, kind) {
            if (args.size() != 1)
                throw std::runtime_error("Binary takes two arguments");
        }
#endif
    };
    struct Reduction : public Composed {
#if !defined(NDEBUG)
        Reduction(BasicCont args, Kind kind) : Composed(args, kind) {
            if (args.size() == 0)
                throw std::runtime_error("Reduction needs at least one argument");
        }
#endif
    };

    struct Add : public Reduction {
        Add(BasicCont args) : Reduction(args, Kind::Add) {}
    };
    struct Mul : public Reduction {
        Mul(BasicCont args) : Reduction(args, Kind::Mul) {}
    };


    struct Sqrt : public Unary {
        Sqrt(BasicCont args) : Unary(args, Kind::Sqrt) {}
    };
    struct Pow : public Binary {
        Pow(BasicCont args) : Binary(args, Kind::Pow) {}
    };
    struct Sub : public Binary {
        Sub(BasicCont args) : Binary(args, Kind::Sub) {}
    };
    struct Div : public Binary {
        Div(BasicCont args) : Binary(args, Kind::Div) {}
    };



    struct NameSpace{
        // no reference counting, objects created in NameSpace live
        // there until the NameSpace instance is deleted.
        std::vector<Basic> instances;
        const idx_t nsymbs;
        NameSpace(int nsymbs) : nsymbs(nsymbs) {
            for (idx_t idx; idx < nsymbs; ++idx)
                instances.push_back(Symbol(idx));
        }
        idx_t mul(std::vector<idx_t> objs){
            BasicCont args;
            for (const auto& idx : objs)
                args.push_back(instances[idx]);
            instances.push_back(Mul(args));
            return instances.size() - 1;
        }
        // NameSpace(NameSpace const&) = delete;
        // NameSpace& operator=(NameSpace const&) = delete;
    };

};

std::size_t sym::calc_hash(BasicCont data) {
    std::size_t result = 0;
    for (const auto& v : data)
        result = result ^ v.get().hash;
    return result;
};

bool sym::lt(data_t arg1, data_t arg2, Kind kind){
    switch(kind){
    case Kind::Symbol:
        return arg1.id < arg2.id;
    case Kind::Integer:
        return arg1.i < arg2.i;
    case Kind::Float:
        return arg1.d < arg2.d;
    }
    auto& c1 = BCAST(arg1.b);
    auto& c2 = BCAST(arg2.b);
    if (c1.size() != c2.size())
        return c1.size() < c2.size();
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        auto& e1 = c1[idx].get();
        auto& e2 = c2[idx].get();
        if (e1.kind != e2.kind)
            return e1.kind < e2.kind;
        if (!(e1 == e2))
            return e1 < e2;
    }
    return false;
}
bool sym::eq(data_t arg1, data_t arg2, Kind kind){
    switch(kind){
    case Kind::Symbol:
        return arg1.id == arg2.id;
    case Kind::Integer:
        return arg1.i == arg2.i;
    case Kind::Float:
        return arg1.d == arg2.d;
    }
    auto& c1 = BCAST(arg1.b);
    auto& c2 = BCAST(arg2.b);
    if (c1.size() != c2.size())
        return false;
    for (std::size_t idx=0; idx < c1.size(); ++idx){
        auto& e1 = c1[idx].get();
        auto& e2 = c2[idx].get();
        if (!(e1 == e2))
            return false;
    }
    return true;
}
#undef BCAST
