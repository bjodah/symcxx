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
#if !defined(NDEBUG)
    std::cout << "Basic::operator ==" << std::endl;
#endif
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
    case Kind::Mul:
        result = 1;
        for (const auto idx: args_from_stack())
            result *= ns->instances[idx].evalf(inp);
        break;
    case Kind::ITE:
        result = (ns->instances[args_from_stack()[0]].evalb(inp)) ?
            ns->instances[args_from_stack()[1]].evalf(inp) :
            ns->instances[args_from_stack()[2]].evalf(inp); break;
    case Kind::Sub:
        result = evalf_arg0() - evalf_arg1(); break;
    case Kind::Div:
        result = evalf_arg0() / evalf_arg1(); break;
    case Kind::Add2:
        result = evalf_arg0() + evalf_arg1(); break;
    case Kind::Mul2:
        result = evalf_arg0() * evalf_arg1(); break;
    case Kind::Cos:
        result = std::cos(evalf_arg0()); break;
    case Kind::Sin:
        result = std::sin(evalf_arg0()); break;
    case Kind::Tan:
        result = std::tan(evalf_arg0()); break;
    case Kind::Acos:
        result = std::acos(evalf_arg0()); break;
    case Kind::Asin:
        result = std::asin(evalf_arg0()); break;
    case Kind::Atan:
        result = std::atan(evalf_arg0()); break;
    case Kind::Atan2:
        result = std::atan2(evalf_arg0(), evalf_arg1()); break;
    case Kind::Cosh:
        result = std::cosh(evalf_arg0()); break;
    case Kind::Sinh:
        result = std::sinh(evalf_arg0()); break;
    case Kind::Tanh:
        result = std::tanh(evalf_arg0()); break;
    case Kind::Acosh:
        result = std::acosh(evalf_arg0()); break;
    case Kind::Asinh:
        result = std::asinh(evalf_arg0()); break;
    case Kind::Atanh:
        result = std::atanh(evalf_arg0()); break;
    case Kind::Exp:
        result = std::exp(evalf_arg0()); break;
    case Kind::Log:
        result = std::log(evalf_arg0()); break;
    case Kind::Log10:
        result = std::exp(evalf_arg0()); break;
    case Kind::Exp2:
        result = std::exp2(evalf_arg0()); break;
    case Kind::Expm1:
        result = std::expm1(evalf_arg0()); break;
    case Kind::Log1p:
        result = std::log1p(evalf_arg0()); break;
    case Kind::Log2:
        result = std::log2(evalf_arg0()); break;
    case Kind::Logb:
        result = std::logb(evalf_arg0()); break;
    case Kind::Pow:
        result = std::pow(evalf_arg0(), evalf_arg1()); break;
    case Kind::Sqrt:
        result = std::sqrt(evalf_arg0()); break;
    case Kind::Cbrt:
        result = std::cbrt(evalf_arg0()); break;
    case Kind::Hypot:
        result = std::hypot(evalf_arg0(), evalf_arg1()); break;
    case Kind::Erf:
        result = std::erf(evalf_arg0()); break;
    case Kind::Erfc:
        result = std::erfc(evalf_arg0()); break;
    case Kind::Tgamma:
        result = std::tgamma(evalf_arg0()); break;
    case Kind::Lgamma:
        result = std::lgamma(evalf_arg0()); break;
    default:
#if !defined(NDEBUG)
        std::cout << "Kind: " << static_cast<int>(kind) << std::endl;
#endif
        throw std::runtime_error("Cannot run evalf for type.");
    }
    return result;
}
bool
symcxx::Basic::evalb(const double inp[]) const {
    auto evalf_arg0 = [&]() -> double { return ns->instances[data.idx_pair.first].evalf(inp); };
    auto evalf_arg1 = [&]() -> double { return ns->instances[data.idx_pair.second].evalf(inp); };
    std::cout << "evalb, " << data.idx_pair.first << ", " << data.idx_pair.second << std::endl;
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
