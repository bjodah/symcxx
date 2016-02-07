#include "symcxx/core.hpp"

const std::vector<symcxx::idx_t>&
symcxx::Basic::args_from_stack() const {
    return ns->args_stack[data.idx_pair.first];
}
bool
symcxx::Basic::operator < (const Basic& other) const {
    if (kind != other.kind)
        return kind < other.kind;
    return lt(data, other.data, kind, ns->args_stack, ns->instances);
}
bool
symcxx::Basic::operator == (const Basic& other) const {
    if (kind != other.kind)
        return false;
    if (hash != other.hash)
        return false;
    return eq(data, other.data, kind, ns->args_stack, ns->instances);
}
// Use < and == to deduce rest of the operators:
bool
symcxx::Basic::operator <= (const Basic& other) const {
    return (*this < other) or (*this == other);
}
bool
symcxx::Basic::operator != (const Basic& other) const {
    return not (*this == other);
}
bool
symcxx::Basic::operator >= (const Basic& other) const {
    return not (*this < other);
}
bool
symcxx::Basic::operator > (const Basic& other) const {
    return not (*this < other) and not (*this == other);
}

std::string
symcxx::Basic::print(const std::vector<std::string>& symbol_names) const {
    std::ostringstream os;
    os << kind_names[static_cast<int>(kind)] + "(";
    switch(kind){
    case Kind::Symbol:
        if (symbol_names.size() == 0)
            os << data.idx_pair.first;
        else
            os << symbol_names[data.idx_pair.first];
        break;
    case Kind::MatProx:
        os << data.idx_pair.first;
        break;
    case Kind::Integer:
        os << data.intgr; break;
    case Kind::Float:
        os << data.dble; break;
    default:
        throw std::runtime_error("Not implemented");
    }
    os << ")";
    return os.str();
}

bool
symcxx::Basic::is_atomic() const {
    return static_cast<int>(kind) <= static_cast<int>(Kind::Float);
}

double
symcxx::Basic::evalf(const double inp[]) const {
    double result;
    auto evalf_arg0 = [&]() -> double { return ns->instances[data.idx_pair.first].evalf(inp); };
    auto evalf_arg1 = [&]() -> double { return ns->instances[data.idx_pair.second].evalf(inp); };
    switch(kind){
// Atomic
    case Kind::Symbol:
        result = inp[data.idx_pair.first]; break;
    case Kind::Float:
        result = data.dble; break;
    case Kind::Integer:
        result = data.intgr; break;
    case Kind::Add:
        result = 0;
        for (const auto idx: args_from_stack())
            result += ns->instances[idx].evalf(inp);
        break;
// With args stack
    case Kind::Mul:
        result = 1;
        for (const auto idx: args_from_stack())
            result *= ns->instances[idx].evalf(inp);
        break;
    case Kind::ITE:
        result = (ns->instances[args_from_stack()[0]].evalb(inp)) ?
            ns->instances[args_from_stack()[1]].evalf(inp) :
            ns->instances[args_from_stack()[2]].evalf(inp); break;
// Unary
    case Kind::Neg:
        result = -evalf_arg0(); break;
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_: \
        result = std::METH_(evalf_arg0()); break;
#include "symcxx/types_nonatomic_unary_math_h.inc"
#undef SYMCXX_TYPE
// Binary
    case Kind::Sub:
        result = evalf_arg0() - evalf_arg1(); break;
    case Kind::Div:
        result = evalf_arg0() / evalf_arg1(); break;
    case Kind::Add2:
        result = evalf_arg0() + evalf_arg1(); break;
    case Kind::Mul2:
        result = evalf_arg0() * evalf_arg1(); break;
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_: \
        result = std::METH_(evalf_arg0(), evalf_arg1()); break;
#include "symcxx/types_nonatomic_binary_math_h.inc"
#undef SYMCXX_TYPE
    default:
        //#if !defined(NDEBUG)
        std::cout << "Kind: " << static_cast<int>(kind) << std::endl;
        //#endif
        throw std::runtime_error("Cannot run evalf for type.");
    }
    return result;
}
bool
symcxx::Basic::evalb(const double inp[]) const {
    auto evalf_arg0 = [&]() -> double { return ns->instances[data.idx_pair.first].evalf(inp); };
    auto evalf_arg1 = [&]() -> double { return ns->instances[data.idx_pair.second].evalf(inp); };
#if !defined(NDEBUG)
    std::cout << "evalb, " << data.idx_pair.first << ", " << data.idx_pair.second << std::endl;
#endif
    switch(kind){
    case Kind::Lt:
        return evalf_arg0()  < evalf_arg1();
    case Kind::Le:
        return evalf_arg0() <= evalf_arg1();
    case Kind::Eq:
        return evalf_arg0() == evalf_arg1();
    case Kind::Ne:
        return evalf_arg0() != evalf_arg1();
    case Kind::Ge:
        return evalf_arg0() >= evalf_arg1();
    case Kind::Gt:
        return evalf_arg0()  > evalf_arg1();
    default:
        throw std::runtime_error("Cannot run evalb for type");
    }
}
