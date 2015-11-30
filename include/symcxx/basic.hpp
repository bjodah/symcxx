#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct Basic {
        const data_t data;
        const Kind kind;
        const hash_t hash;
        const NameSpace * const ns;
        Basic(const hash_t hash, const Kind kind, const data_t data, const NameSpace * const ns) :
            data(data), kind(kind), hash(hash), ns(ns) {}
        // Basic(Basic const&) = delete;
        // Basic& operator=(Basic const&) = delete;

        const std::vector<idx_t>& args() const {
            return ns->args_stack[data.id];
        }
        bool operator < (const Basic& other) const {
            if (kind != other.kind)
                return kind < other.kind;
            return lt(data, other.data, kind, ns->args_stack, ns->instances);
        }
        bool operator == (const Basic& other) const {
            if (kind != other.kind)
                return false;
            if (hash != other.hash)
                return false;
            return eq(data, other.data, kind, ns->args_stack, ns->instances);
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
            auto arg_evalf = [&](int argid) -> double { return ns->instances[args()[argid]].evalf(inp); };
            auto arg_evalb = [&](int argid) -> double { return ns->instances[args()[argid]].evalb(inp); };
            switch(kind){
            case Kind::Symbol:
                result = inp[data.id]; break;
            case Kind::Float:
                result = data.d; break;
            case Kind::Integer:
                result = data.i; break;
            case Kind::Add:
                result = 0;
                for (const auto idx: args())
                    result += arg_evalf(idx);
                break;
            case Kind::Mul:
                result = 1;
                for (const auto idx: args())
                    result *= arg_evalf(idx);
                break;
            case Kind::Sub:
                result = arg_evalf(0) - arg_evalf(1); break;
            case Kind::Div:
                result = arg_evalf(0) / arg_evalf(1); break;
            case Kind::Cos:
                result = std::cos(arg_evalf(0)); break;
            case Kind::Sin:
                result = std::sin(arg_evalf(0)); break;
            case Kind::Tan:
                result = std::tan(arg_evalf(0)); break;
            case Kind::Acos:
                result = std::acos(arg_evalf(0)); break;
            case Kind::Asin:
                result = std::asin(arg_evalf(0)); break;
            case Kind::Atan:
                result = std::atan(arg_evalf(0)); break;
            case Kind::Atan2:
                result = std::atan2(arg_evalf(0), arg_evalf(1)); break;
            case Kind::Cosh:
                result = std::cosh(arg_evalf(0)); break;
            case Kind::Sinh:
                result = std::sinh(arg_evalf(0)); break;
            case Kind::Tanh:
                result = std::tanh(arg_evalf(0)); break;
            case Kind::Acosh:
                result = std::acosh(arg_evalf(0)); break;
            case Kind::Asinh:
                result = std::asinh(arg_evalf(0)); break;
            case Kind::Atanh:
                result = std::atanh(arg_evalf(0)); break;
            case Kind::Exp:
                result = std::exp(arg_evalf(0)); break;
            case Kind::Log:
                result = std::log(arg_evalf(0)); break;
            case Kind::Log10:
                result = std::exp(arg_evalf(0)); break;
            case Kind::Exp2:
                result = std::exp2(arg_evalf(0)); break;
            case Kind::Expm1:
                result = std::expm1(arg_evalf(0)); break;
            case Kind::Log1p:
                result = std::log1p(arg_evalf(0)); break;
            case Kind::Log2:
                result = std::log2(arg_evalf(0)); break;
            case Kind::Logb:
                result = std::logb(arg_evalf(0)); break;
            case Kind::Pow:
                result = std::pow(arg_evalf(0), arg_evalf(1)); break;
            case Kind::Sqrt:
                result = std::sqrt(arg_evalf(0)); break;
            case Kind::Cbrt:
                result = std::cbrt(arg_evalf(0)); break;
            case Kind::Hypot:
                result = std::hypot(arg_evalf(0), arg_evalf(1)); break;
            case Kind::Erf:
                result = std::erf(arg_evalf(0)); break;
            case Kind::Erfc:
                result = std::erfc(arg_evalf(0)); break;
            case Kind::Tgamma:
                result = std::tgamma(arg_evalf(0)); break;
            case Kind::Lgamma:
                result = std::lgamma(arg_evalf(0)); break;
            case Kind::ITE:
                result = (arg_evalb(0)) ? arg_evalf(1) : arg_evalf(2); break;
            default:
                throw std::runtime_error("Cannot run evalf for type.");
            }
            return result;
        }
        bool evalb(const double inp[], const ArgStack_t& args_stack, const std::vector<Basic>& instances) const {
            auto arg_evalf = [&](int argid) -> double { return ns->instances[args()[argid]].evalf(inp); };
            switch(kind){
            case Kind::Lt:
                return (arg_evalf(0))  < (arg_evalf(1));
            case Kind::Le:
                return (arg_evalf(0)) <= (arg_evalf(1));
            case Kind::Eq:
                return (arg_evalf(0)) == (arg_evalf(1));
            case Kind::Ne:
                return (arg_evalf(0)) != (arg_evalf(1));
            case Kind::Ge:
                return (arg_evalf(0)) >= (arg_evalf(1));
            case Kind::Gt:
                return (arg_evalf(0))  > (arg_evalf(1));
            default:
                throw std::runtime_error("Cannot run evalb for type");
            }
        }
    };
}
