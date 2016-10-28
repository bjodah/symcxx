#include "symcxx/core.hpp"

symcxx::NameSpace::NameSpace(idx_t n_pre_symbs) : n_pre_symbs(n_pre_symbs), n_symbs(n_pre_symbs) {
    instances.reserve(2*(n_pre_intgrs + n_special + n_pre_symbs));  // arbitrary, "2" could be optimized
    for (idx_t idx=0; idx < (n_pre_intgrs+1)/2; ++idx)  //
        instances.push_back(Integer(idx, this));
    for (idx_t idx=0; idx < n_pre_intgrs/2; ++idx)
        instances.push_back(Integer(-static_cast<intgr_t>(n_pre_intgrs/2 - idx), this));
    make_float(3.1415926535897932385); // pi
    make_float(-3.1415926535897932385); // -pi
    make_float(2.7182818284590452354); // exp(1)
    make_float(-2.7182818284590452354); // -exp(1)
    make_float(0.69314718055994530942); // log(2)
    make_float(2.3025850929940456840); // log(10)
    for (idx_t idx=0; idx < n_pre_symbs; ++idx)
        instances.push_back(Symbol(idx, this));
}

symcxx::idx_t
symcxx::NameSpace::reg_args(const std::vector<idx_t>& objs) {
    args_stack.push_back(objs);
    return args_stack.size() - 1;
}

bool
symcxx::NameSpace::in_namespace(const Basic& looking_for, idx_t * idx) const {
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

bool
symcxx::NameSpace::has(const idx_t idx, const idx_t looking_for) const {
    if (idx == looking_for)
        return true;
    const auto& inst = instances[idx];
    std::ostringstream os;
    switch(inst.kind){
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_atomic.inc"
        return false;
#include "symcxx/types_nonatomic_unary.inc"
        return has(inst.data.idx_pair.first, looking_for);
#include "symcxx/types_nonatomic_binary.inc"
        return (has(inst.data.idx_pair.first, looking_for) ||
                has(inst.data.idx_pair.second, looking_for)) ? true : false;
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE
        for (auto i : args_stack[inst.data.idx_pair.first]){
            if (has(i, looking_for))
                return true;
        }
        return false;
    case Kind::Kind_Count:
        throw std::runtime_error("Kind_Count is only a place-holder");
    default:
        throw std::runtime_error("Unhandled kind");
    }
}

bool
symcxx::NameSpace::equal(const idx_t idx_a, const idx_t idx_b) const{
    const auto& a = instances[idx_a];
    const auto& b = instances[idx_b];
    return a == b;
}

bool
symcxx::NameSpace::is_zero(const idx_t idx) const {
    if (idx == 0)
        return true;
    if ((instances[idx].kind == Kind::Float && instances[idx].data.dble == 0))
        return true;
    else
        return false;
}


bool
symcxx::NameSpace::is_one(const idx_t idx) const {
    if (idx == 1)
        return true;
    if ((instances[idx].kind == Kind::Float && instances[idx].data.dble == 1))
        return true;
    else
        return false;
}

bool
symcxx::NameSpace::apparently_negative(const idx_t idx) const {
    if (idx > n_pre_intgrs/2 && idx < n_pre_intgrs)
        return true;
    switch(instances[idx].kind){
    case Kind::Neg:
        return true;  // this does not guarantee it's negative but will flatten tree.
    case Kind::Float:
        return instances[idx].data.dble < 0;
    default:
        return false;
    }
}


symcxx::idx_t
symcxx::NameSpace::make_symbol(idx_t symb_idx){
    if (symb_idx < n_pre_symbs)
        return n_pre_intgrs + n_special + symb_idx;
    const auto instance = Symbol(symb_idx, this);
    idx_t idx;
    if (in_namespace(instance, &idx)){
#if !defined(NDEBUG)
        std::cout << "make_symbol(" << symb_idx << ") - old!" << std::endl;
#endif
        // throw std::runtime_error("Something fishy about that call..");
        return idx;
    }
#if !defined(NDEBUG)
    std::cout << "make_symbol(" << symb_idx << ") - new!" << std::endl;
#endif
    if (symb_idx != n_symbs){
        throw std::runtime_error("Something fishy about skipping symbols..");
    }
    n_symbs++;
    instances.push_back(instance);
    return instances.size() - 1;
}

symcxx::idx_t
symcxx::NameSpace::make_symbol(){
    return make_symbol(n_symbs);
}

std::vector<symcxx::idx_t>
symcxx::NameSpace::make_symbols(symcxx::idx_t n){
    std::vector<symcxx::idx_t> args;
    const auto cur_n_symbs = n_symbs;
    for (idx_t i=cur_n_symbs; i<cur_n_symbs+n; ++i)
        args.push_back(make_symbol(i));
    return args;
}

symcxx::idx_t
symcxx::NameSpace::make_integer(intgr_t i){
    if (i >= 0 && static_cast<idx_t>(i) < (n_pre_intgrs+1)/2)
        return i;
    if (i < 0 && static_cast<idx_t>(-i) < n_pre_intgrs/2)
        return n_pre_intgrs + i;
    const auto instance = Integer(i, this);
    idx_t idx;
    if (in_namespace(instance, &idx)){
#if !defined(NDEBUG)
        std::cout << "make_integer(" << i << ") - old!" << std::endl;
#endif
        return idx;
    }
#if !defined(NDEBUG)
    std::cout << "make_integer(" << i << ") - new!" << std::endl;
#endif
    instances.push_back(instance);
    return instances.size() - 1;
}

symcxx::idx_t
symcxx::NameSpace::make_float(double f){
    const auto instance = Float(f, this);
    idx_t idx;
    if (in_namespace(instance, &idx))
        return idx;
    instances.push_back(instance);
    return instances.size() - 1;
}
symcxx::idx_t
symcxx::NameSpace::make_nan(){
    return make_float(std::nan(""));
}

symcxx::idx_t
symcxx::NameSpace::make_matrix(idx_t nr, idx_t nc, std::vector<idx_t> data){
    matrices.push_back(Matrix(nr, nc, data));
    const auto instance = MatProx(matrices.size() - 1, this);
    instances.push_back(instance);
    return instances.size() - 1;
}

symcxx::idx_t
symcxx::NameSpace::matrix_jacobian(idx_t idx_y, idx_t idx_dx){
    matrices.push_back(matrices[instances[idx_y].data.idx_pair.first].jacobian(
        matrices[instances[idx_dx].data.idx_pair.first], this));
    const auto instance = MatProx(matrices.size() - 1, this);
    instances.push_back(instance);
    return instances.size() - 1;
}

void
symcxx::NameSpace::matrix_evalf(idx_t idx, const double * const inp, double * const out) const {
    matrices[instances[idx].data.idx_pair.first].evalf_all(inp, out, this);
}

symcxx::idx_t
symcxx::NameSpace::matrix_get_nr(idx_t idx) const {
    const auto& mat = matrices[instances[idx].data.idx_pair.first];
    return mat.nr;
}

symcxx::idx_t
symcxx::NameSpace::matrix_get_nc(idx_t idx) const {
    const auto& mat = matrices[instances[idx].data.idx_pair.first];
    return mat.nc;
}
symcxx::idx_t
symcxx::NameSpace::matrix_get_elem(idx_t idx, idx_t ri, idx_t ci) const {
    const auto& mat = matrices[instances[idx].data.idx_pair.first];
    return mat.data[ri*matrix_get_nc(idx) + ci];
}

symcxx::idx_t
symcxx::NameSpace::factor(idx_t idx) {
    //auto * inst = &instances[idx];
    if (instances[idx].kind != Kind::Integer){
        throw std::runtime_error("Not an integer!");
    }
    const intgr_t upper = std::ceil<int64_t>(std::sqrt(instances[idx].data.intgr));
    std::vector<idx_t> args;
    while(instances[idx].data.intgr % 2 == 0){
        args.push_back(make_integer(2));
        idx = make_integer(instances[idx].data.intgr/2);
    }
    for (intgr_t cand=3; cand < upper; cand += 2){
        while(instances[idx].data.intgr % cand == 0){
            args.push_back(make_integer(cand));
            idx = make_integer(instances[idx].data.intgr/cand);
        }
    }
    if (args.size() == 0){
        return idx;
    } else {
        args.push_back(idx);
        return create(Kind::Mul, args);
    }
}

std::string
symcxx::NameSpace::print_node(const idx_t idx, const std::vector<std::string>& symbol_names) const {
    const auto& inst = instances[idx];
    std::ostringstream os;
    bool first = true;
    switch(inst.kind){
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_atomic.inc"
        os << inst.print(symbol_names); break;
#include "symcxx/types_nonatomic_unary.inc"
        os << kind_names[static_cast<int>(inst.kind)] + "(" <<
            inst.data.idx_pair.first << ")"; break;
#include "symcxx/types_nonatomic_binary.inc"
        os << kind_names[static_cast<int>(inst.kind)] + "(" <<
            inst.data.idx_pair.first << ", " <<
            inst.data.idx_pair.second << ")"; break;
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE
        os << kind_names[static_cast<int>(inst.kind)] + "(";
        for (auto i : args_stack[inst.data.idx_pair.first]){
            if (first)
                first = false;
            else
                os << ", ";
            os << i;
        }
        os << ")";
        break;
    case Kind::Kind_Count:
        break;
    }
    return os.str();
}

std::string
symcxx::NameSpace::print_ast(const idx_t idx, const std::vector<std::string>& symbol_names) const {
    const auto& inst = instances[idx];
    std::ostringstream os;
    bool first = true;
    switch(inst.kind){
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_atomic.inc"
        os << inst.print(symbol_names); break;
#include "symcxx/types_nonatomic_unary.inc"
        os << kind_names[static_cast<int>(inst.kind)] + "(" <<
            print_ast(inst.data.idx_pair.first, symbol_names) << ")"; break;
#include "symcxx/types_nonatomic_binary.inc"
        os << kind_names[static_cast<int>(inst.kind)] + "(" <<
            print_ast(inst.data.idx_pair.first, symbol_names) << ", " <<
            print_ast(inst.data.idx_pair.second, symbol_names) << ")"; break;
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE
        os << kind_names[static_cast<int>(inst.kind)] + "(";
        for (auto i : args_stack[inst.data.idx_pair.first]){
            if (first)
                first = false;
            else
                os << ", ";
            os << print_ast(i, symbol_names);
        }
        os << ")";
        break;
    case Kind::Kind_Count:
        break;
    }
    return os.str();
}

symcxx::idx_t
symcxx::NameSpace::rebuild_idx_into_ns(const idx_t idx, NameSpace& ns, const std::vector<idx_t>& symb_mapping) const {
    const auto& inst = instances[idx];
    std::vector<idx_t> args;
    switch(inst.kind){
    case Kind::Symbol:
        return ns.n_pre_intgrs + ns.n_special + std::find(symb_mapping.begin(), symb_mapping.end(),
                                                          idx) - symb_mapping.begin();
    case Kind::Integer:
        return ns.make_integer(inst.data.intgr);
    case Kind::MatProx:
        for (idx_t inner : matrices[inst.data.idx_pair.first].data){
            args.push_back(rebuild_idx_into_ns(inner, ns, symb_mapping));
        }
        return ns.make_matrix(matrices[inst.data.idx_pair.first].nr,
                              matrices[inst.data.idx_pair.first].nc, args);
    case Kind::Float:
        return ns.make_float(inst.data.dble);
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_:
#include "symcxx/types_nonatomic_unary.inc"
        return ns.create(inst.kind, rebuild_idx_into_ns(inst.data.idx_pair.first, ns, symb_mapping));
#include "symcxx/types_nonatomic_binary.inc"
        return ns.create(inst.kind,
                         rebuild_idx_into_ns(inst.data.idx_pair.first, ns, symb_mapping),
                         rebuild_idx_into_ns(inst.data.idx_pair.second, ns, symb_mapping));
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE
        for (idx_t inner : args_stack[inst.data.idx_pair.first]){
            args.push_back(rebuild_idx_into_ns(inner, ns, symb_mapping));
        }
        return ns.create(inst.kind, args);
    case Kind::Kind_Count:
        throw std::runtime_error("Kind_Count not valid.");
    default: //case Kind::Kind_Count:
        throw std::runtime_error("Bug: unhandled kind.");
    }
}

std::unique_ptr<symcxx::NameSpace>
symcxx::NameSpace::rebuild(const std::vector<idx_t>& args,
                           const std::vector<idx_t>& exprs, idx_t nr, idx_t nc) const {
    auto ns = make_unique<NameSpace>(args.size());
    std::vector<idx_t> new_exprs;
    for (auto expr : exprs) {
        new_exprs.push_back(rebuild_idx_into_ns(expr, *ns, args));
    }
    ns->make_matrix(nr, nc, new_exprs);
    return ns;
}

std::unique_ptr<symcxx::NameSpace>
symcxx::NameSpace::rebuild_from_matrix(const std::vector<idx_t>& args, idx_t mat_idx) const {
    const Matrix& mat = matrices[instances[mat_idx].data.idx_pair.first];
    return rebuild(args, mat.data, mat.nr, mat.nc);
}


#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    symcxx::idx_t symcxx::NameSpace::METH_(const std::vector<symcxx::idx_t>& args){ \
        const auto instance = CLS_(reg_args(args), this);                \
        idx_t idx;                                                      \
        if (in_namespace(instance, &idx)){                                       \
            args_stack.pop_back();                                      \
            return idx;                                                 \
        }                                                               \
        instances.push_back(instance);                                  \
        return instances.size() - 1;                                    \
    }
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE


#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    symcxx::idx_t symcxx::NameSpace::METH_(const symcxx::idx_t inst){ \
        const auto instance = CLS_(inst, this);                          \
        idx_t idx;                                                      \
        if (in_namespace(instance, &idx)){                                       \
            return idx;                                                 \
        }                                                               \
        instances.push_back(instance);                                  \
        return instances.size() - 1;                                    \
    }
#include "symcxx/types_nonatomic_unary.inc"
#undef SYMCXX_TYPE

#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    symcxx::idx_t symcxx::NameSpace::METH_(const symcxx::idx_t inst0, const symcxx::idx_t inst1){  \
        const auto instance = CLS_(inst0, inst1, this);                  \
        idx_t idx;                                                      \
        if (in_namespace(instance, &idx)){                                       \
            return idx;                                                 \
        }                                                               \
        instances.push_back(instance);                                  \
        return instances.size() - 1;                                    \
    }
#include "symcxx/types_nonatomic_binary.inc"
#undef SYMCXX_TYPE

symcxx::idx_t
symcxx::NameSpace::create(const Kind kind, const std::vector<idx_t>& args){
#if !defined(NDEBUG)
    std::cout << "Creating(args-vec) kind=" << kind_names[static_cast<int>(kind)] << ", with args[" << args.size() << "]: ";
    for (auto obj : args)
        std::cout << obj << ", ";
    std::cout << std::endl;
#endif
    const auto zero = make_integer(0);
    const auto one = make_integer(1);
    std::vector<idx_t> new_args;
    switch(kind){
    case Kind::Add:
        if (args.size() == 0)
            throw std::runtime_error("create Add from length 0 vector of arguments");
        new_args = merge_drop_sort_collect(args, Kind::Mul, {zero, mul({zero})}, Kind::Add);
        if (new_args.size() == 0)
            return zero;
        else if (new_args.size() == 1)
            return new_args[0];
        else if (new_args.size() == 2)
            return add2(new_args[0], new_args[1]);
        else
            return add(new_args);
    case Kind::Mul:
        if (args.size() == 0)
            throw std::runtime_error("create Mul from length 0 vector of arguments");
        if (std::find(args.begin(), args.end(), zero) != args.end())
            return zero; // we found a zero among the factors
        new_args = merge_drop_sort_collect(args, Kind::Pow, {one}, Kind::Mul);
        if (new_args.size() == 1)
            return new_args[0];
        else if (new_args.size() == 2)
            return mul2(new_args[0], new_args[1]);
        else
            return mul(new_args);
    case Kind::ITE:
        if (args.size() != 3)
            throw std::runtime_error("create ITE from vector of length != 3");
        else if (args[1] == args[2])
            return args[1];
        else
            return ite(args);
    default:
#if !defined(NDEBUG)
        std::cout << "create(vector) does not support kind:" << kind_names[static_cast<int>(kind)] << std::endl;
#endif
        throw std::runtime_error("create(vector) does not support kind.");
    }
}

symcxx::idx_t
symcxx::NameSpace::create(const Kind kind, const idx_t inst_idx){
#ifndef NDEBUG
    std::cout << "Creating(unary) kind=" << kind_names[static_cast<int>(kind)] << ", with inst_idx=" << inst_idx << std::endl;
#endif
    switch(kind){
    case Kind::Cos:
        if (is_zero(inst_idx))
            return make_integer(1);
        if (inst_idx == pi_id || inst_idx == neg_pi_id)
            return make_integer(-1);
        return cos(inst_idx);
    case Kind::Sin:
        if (is_zero(inst_idx) || inst_idx == pi_id || inst_idx == neg_pi_id)
            return make_integer(0);
        return sin(inst_idx);
    case Kind::Tan:
        if (is_zero(inst_idx) || inst_idx == pi_id || inst_idx == neg_pi_id)
            return make_integer(0);
        return tan(inst_idx);
    case Kind::Acos:
        return acos(inst_idx);
    case Kind::Asin:
        return asin(inst_idx);
    case Kind::Atan:
        return atan(inst_idx);
    case Kind::Cosh:
        if (is_zero(inst_idx))
            return make_integer(1);
        return cosh(inst_idx);
    case Kind::Sinh:
        if (is_zero(inst_idx))
            return make_integer(0);
        return sinh(inst_idx);
    case Kind::Tanh:
        if (is_zero(inst_idx))
            return make_integer(0);
        return tanh(inst_idx);
    case Kind::Acosh:
        if (is_one(inst_idx))
            return make_integer(0);
        return acosh(inst_idx);
    case Kind::Asinh:
        if (is_zero(inst_idx))
            return make_integer(0);
        return asinh(inst_idx);
    case Kind::Atanh:
        if (is_zero(inst_idx))
            return make_integer(0);
        return atanh(inst_idx);
    case Kind::Exp:
        if (is_zero(inst_idx))
            return make_integer(1);
        if (instances[inst_idx].kind == Kind::Log)
            return instances[inst_idx].data.idx_pair.first;
        return exp(inst_idx);
    case Kind::Log:
        if (inst_idx == e_id)
            return make_integer(1);
        else if (inst_idx == neg_e_id)
            return make_integer(-1);
        if (instances[inst_idx].kind == Kind::Exp)
            return instances[inst_idx].data.idx_pair.first;
        return log(inst_idx);
    case Kind::Log10:
        return log10(inst_idx);
    case Kind::Exp2:
        return exp2(inst_idx);
    case Kind::Expm1:
        return expm1(inst_idx);
    case Kind::Log1p:
        return log1p(inst_idx);
    case Kind::Log2:
        return log2(inst_idx);
    case Kind::Sqrt:
        if (instances[inst_idx].kind == Kind::Pow)
            if (instances[inst_idx].data.idx_pair.second == make_integer(2))
                return instances[inst_idx].data.idx_pair.first;
        return sqrt(inst_idx);
    case Kind::Cbrt:
        if (instances[inst_idx].kind == Kind::Pow)
            if (instances[inst_idx].data.idx_pair.second == make_integer(3))
                return instances[inst_idx].data.idx_pair.first;
        return cbrt(inst_idx);
    case Kind::Erf:
        return erf(inst_idx);
    case Kind::Erfc:
        return erfc(inst_idx);
    case Kind::Tgamma:
        return tgamma(inst_idx);
    case Kind::Lgamma:
        return lgamma(inst_idx);
    case Kind::Neg:
        if (inst_idx == pi_id)
            return neg_pi_id;
        else if (inst_idx == neg_pi_id)
            return pi_id;
        else if (inst_idx == e_id)
            return neg_e_id;
        else if (inst_idx == neg_e_id)
            return e_id;

        switch(instances[inst_idx].kind){
        case Kind::Integer:
            return make_integer(-instances[inst_idx].data.intgr);
        case Kind::Float:
            return make_float(-instances[inst_idx].data.dble);
        case Kind::Neg:
            return instances[inst_idx].data.idx_pair.first;
        case Kind::Sub:
            return sub(instances[inst_idx].data.idx_pair.second,
                       instances[inst_idx].data.idx_pair.first);
        default:
            return neg(inst_idx);
        }
    default:
#if !defined(NDEBUG)
        std::cout << "create(unary) does not support kind:" << kind_names[static_cast<int>(kind)] << std::endl;
#endif
        throw std::runtime_error("create(unary) does not support kind.");
    }
}

symcxx::idx_t
symcxx::NameSpace::create(const Kind kind, const idx_t inst_idx0, const idx_t inst_idx1){
#ifndef NDEBUG
    std::cout << "Creating(binary) kind=" << kind_names[static_cast<int>(kind)] << ", with inst_idx0=" << inst_idx0 <<
        ", inst_idx1=" << inst_idx1 << std::endl;
#endif
    auto are_sorted = [&] () -> bool {
#ifndef NDEBUG
        std::cout << "are_sorted()";
#endif
        bool result = instances[inst_idx0] < instances[inst_idx1];
#ifndef NDEBUG
        std::cout << " -> " << result << std::endl;
#endif
        return result;
    };
    switch(kind){
    case Kind::Add:
    case Kind::Add2:
        return create(Kind::Add, std::vector<idx_t>{{inst_idx1, inst_idx0}});
    case Kind::Mul:
    case Kind::Mul2:
        return create(Kind::Mul, std::vector<idx_t>{{inst_idx1, inst_idx0}});
    case Kind::Sub:
        if (inst_idx0 == inst_idx1)
            return make_integer(0);
        else
            if (is_zero(inst_idx0))
                return neg(inst_idx1);
            if (is_zero(inst_idx1))
                return inst_idx0;
            if (apparently_negative(inst_idx1))
                return create(Kind::Add, inst_idx0, create(Kind::Neg, inst_idx1));
            return sub(inst_idx0, inst_idx1);
    case Kind::Div:
        if (is_zero(inst_idx1))
            return make_nan();
        else
            if (is_zero(inst_idx0))
                return make_integer(0);
            else
                if (is_one(inst_idx1))
                    return inst_idx0;
                else
                    return div(inst_idx0, inst_idx1);
    case Kind::Pow:
        if (is_zero(inst_idx1))
            return make_integer(1);
        else if (is_one(inst_idx1))
            return inst_idx0;
        else if (is_zero(inst_idx0))
            return make_integer(0);
        else
            return pow(inst_idx0, inst_idx1);
    case Kind::Atan2:
            return atan2(inst_idx0, inst_idx1);
    case Kind::Hypot:
        if (are_sorted())
            return hypot(inst_idx0, inst_idx1);
        else
            return hypot(inst_idx1, inst_idx0);
#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    case Kind::CLS_: \
        return METH_(inst_idx0, inst_idx1);
#include "symcxx/types_nonatomic_binary_relational.inc"
#undef SYMCXX_TYPE
    default:
#if !defined(NDEBUG)
        std::cout << "create(binary) does not support kind:" << kind_names[static_cast<int>(kind)] << std::endl;
#endif
        throw std::runtime_error("create(binary) does not support kind.");
    }
}


double
symcxx::NameSpace::evalf(const idx_t id, const double inp[]) const {
    return instances[id].evalf(inp);
}

bool
symcxx::NameSpace::evalb(const idx_t id, const double inp[]) const {
    return instances[id].evalb(inp);
}

symcxx::idx_t
symcxx::NameSpace::diff(const idx_t inst_id, const idx_t wrt_id)
{
    const Basic& inst = instances[inst_id];
    const Basic& wrt = instances[wrt_id];
    std::vector<idx_t> args;
    std::vector<idx_t> inner_args;
    const std::vector<idx_t>& args_from_stack = inst.args_from_stack();
#if !defined(NDEBUG)
    std::cout << "Diff on inst=" << inst_id << "(kind=" << kind_names[static_cast<int>(inst.kind)] << ")";
    std::cout << ", wrt to " << wrt_id << std::endl; // << ", idx=" << idx(&inst) << ", wrt=" << idx(&wrt) << std::endl;
#endif
    switch(inst.kind){
    case Kind::Symbol:
        if (inst == wrt)
            return make_integer(1);
        else
            return make_integer(0);
    case Kind::Integer:
        return make_integer(0);
    case Kind::Float:
        return make_float(0);
    case Kind::Add:
        for (const auto idx : args_from_stack){
            args.push_back(diff(idx, wrt_id));
        }
        return create(Kind::Add, args);
    case Kind::Mul:
        for (const auto idx : args_from_stack){
            inner_args = {{}};
            for (idx_t inner_idx=0; inner_idx < args_from_stack.size(); ++inner_idx){
                if (inner_idx == idx)
                    inner_args.push_back(diff(args_from_stack[inner_idx], wrt_id));
                else
                    inner_args.push_back(args_from_stack[inner_idx]);
            }
            args.push_back(create(Kind::Mul, inner_args));
        }
        return create(Kind::Add, args);
    case Kind::Sub:
        return create(Kind::Sub,
                      diff(inst.data.idx_pair.first, wrt_id),
                      diff(inst.data.idx_pair.second, wrt_id));
    case Kind::Div:
        return create(Kind::Div,
                      create(Kind::Sub,
                             create(Kind::Mul,
                                    diff(inst.data.idx_pair.first, wrt_id),
                                    inst.data.idx_pair.second
                                    ),
                             create(Kind::Mul,
                                    inst.data.idx_pair.first,
                                    diff(inst.data.idx_pair.second, wrt_id)
                                    )
                             ),
                      create(Kind::Pow,
                             inst.data.idx_pair.second,
                             make_integer(2))
                      );
    case Kind::Add2:
        return create(Kind::Add,
                      diff(inst.data.idx_pair.first, wrt_id),
                      diff(inst.data.idx_pair.second, wrt_id));
    case Kind::Mul2:
        return create(Kind::Add,
                      create(Kind::Mul,
                             diff(inst.data.idx_pair.first, wrt_id),
                             inst.data.idx_pair.second
                             ),
                      create(Kind::Mul,
                             inst.data.idx_pair.first,
                             diff(inst.data.idx_pair.second, wrt_id)
                             )
                      );
    case Kind::Pow:
        {
            const auto base = inst.data.idx_pair.first;
            const auto exponent = inst.data.idx_pair.second;
            const bool in_base = has(base, wrt_id);
            const bool in_exponent = has(exponent, wrt_id);
            if (in_base){
                if (!in_exponent){
                    return create(Kind::Mul, {{
                                exponent,
                                    create(Kind::Pow, base, create(Kind::Sub, exponent, make_integer(1))),
                                    diff(base, wrt_id)
                                    }});
                }
            } else {
                if (!in_exponent){
                    return make_integer(0);
                }
            }
            return diff(create(Kind::Exp,
                               create(Kind::Mul,
                                      create(Kind::Log, base),
                                      exponent)),
                        wrt_id);
        }
    case Kind::Neg:
        return create(Kind::Neg, diff(inst.data.idx_pair.first, wrt_id));
    case Kind::Cos:
        return create(Kind::Mul, {{
                    make_integer(-1),
                        create(Kind::Sin,
                               inst.data.idx_pair.first),
                        diff(inst.data.idx_pair.first, wrt_id)
                        }}
            );
    case Kind::Sin:
        return create(Kind::Mul,
                      create(Kind::Cos,
                             inst.data.idx_pair.first),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Tan:
        return create(Kind::Mul,
                      create(Kind::Add,
                             make_integer(1),
                             create(Kind::Pow,
                                    create(Kind::Tan,
                                           inst.data.idx_pair.first),
                                    make_integer(2))
                             ),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Acos:
    case Kind::Asin:
        return create(Kind::Mul,
                      create(Kind::Div,
                             make_integer((inst.kind == Kind::Asin) ? 1 : -1),
                             create(Kind::Sqrt,
                                    create(Kind::Sub,
                                           make_integer(1),
                                           create(Kind::Pow,
                                                  inst.data.idx_pair.first,
                                                  make_integer(2)
                                                  )
                                           )
                                    )
                             ),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Atan:
        return create(Kind::Div,
                      diff(inst.data.idx_pair.first, wrt_id),
                      create(Kind::Add,
                             make_integer(1),
                             create(Kind::Pow,
                                    inst.data.idx_pair.first,
                                    make_integer(2)))
                      );
    case Kind::Cosh:
        return create(Kind::Mul,
                      create(Kind::Sinh,
                             inst.data.idx_pair.first),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Sinh:
        return create(Kind::Mul,
                      create(Kind::Cosh,
                             inst.data.idx_pair.first),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Tanh:
        return create(Kind::Mul,
                      create(Kind::Sub,
                             make_integer(1),
                             create(Kind::Pow,
                                    inst_id,
                                    make_integer(2))),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Acosh:
    case Kind::Asinh:
        return create(Kind::Mul,
                      create(Kind::Div,
                             make_integer(1),
                             create(Kind::Sqrt,
                                    create(Kind::Add,
                                           make_integer((inst.kind == Kind::Asinh) ? 1 : -1),
                                           create(Kind::Pow,
                                                  inst.data.idx_pair.first,
                                                  make_integer(2)
                                                  )
                                           )
                                    )
                             ),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Atanh:
        return create(Kind::Mul,
                      create(Kind::Div,
                             make_integer(1),
                                    create(Kind::Sub,
                                           make_integer(1),
                                           create(Kind::Pow,
                                                  inst.data.idx_pair.first,
                                                  make_integer(2)
                                                  )
                                           )
                             ),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Exp:
        return create(Kind::Mul,
                      inst_id,
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Log:
        return create(Kind::Div,
                      diff(inst.data.idx_pair.first, wrt_id),
                      inst.data.idx_pair.first
                      );
    case Kind::Log10:
        return create(Kind::Div,
                      diff(inst.data.idx_pair.first, wrt_id),
                      create(Kind::Mul, inst.data.idx_pair.first, ln10_id)
                      );
    case Kind::Exp2:
        return create(Kind::Mul, {{ inst_id, ln2_id,
                        diff(inst.data.idx_pair.first, wrt_id) }});
    case Kind::Expm1:
        return create(Kind::Mul,
                      create(Kind::Exp, inst.data.idx_pair.first),
                      diff(inst.data.idx_pair.first, wrt_id)
                      );
    case Kind::Sqrt:
        return create(Kind::Div,
                      diff(inst.data.idx_pair.first, wrt_id),
                      create(Kind::Mul,
                             inst_id,
                             make_integer(2))
                      );
    case Kind::Cbrt:
        return create(Kind::Mul,
                      diff(inst.data.idx_pair.first, wrt_id),
                      create(Kind::Div,
                             create(Kind::Pow,
                                    inst.data.idx_pair.first,
                                    create(Kind::Div,
                                           make_integer(-2),
                                           make_integer(3))
                                    ),
                             make_integer(3))
                      );
    case Kind::Erf:
    case Kind::Erfc:
        return create(Kind::Mul,
                      diff(inst.data.idx_pair.first, wrt_id),
                      create(Kind::Div,
                             create(Kind::Mul,
                                    make_integer((inst.kind == Kind::Erf) ? 2 : -2),
                                    create(Kind::Exp,
                                           create(Kind::Neg,
                                                  create(Kind::Pow,
                                                         inst.data.idx_pair.first,
                                                         make_integer(2)
                                                         )))),
                             create(Kind::Sqrt,
                                    pi_id)
                             )
                      );
    default:
        std::cout << "Unsupported kind: " << kind_names[static_cast<int>(inst.kind)] << std::endl;
        throw std::runtime_error("diff does not support kind.");
    }
}

std::vector<symcxx::idx_t>
symcxx::NameSpace::collect(const Kind collect_to_kind, const std::vector<idx_t>& sorted_args) {
#if !defined(NDEBUG)
    std::cout << "collect(collect_to_kind=" << kind_names[static_cast<int>(collect_to_kind)] << ", sorted_args="<< sorted_args<<")";
#endif
    const idx_t nargs = sorted_args.size();
    if (nargs <= 1)
        return sorted_args;
    idx_t prev = sorted_args[0];
    idx_t count = 1;
    std::vector<idx_t> new_args;
    bool is_last;
    auto add = [&] (idx_t arg_, idx_t cnt_) -> void {
        if (cnt_ == 1)
            new_args.push_back(arg_);
        else if (cnt_ > 1)
            new_args.push_back(create(collect_to_kind, arg_, make_integer(cnt_)));
    };
    for (idx_t idx=1; idx<nargs; ++idx){
        const auto arg = sorted_args[idx];
        is_last = (idx == (nargs - 1));
        if (arg == prev){
            count++;
            if (is_last)
                add(arg, count);
        } else {
            add(prev, count);
            if (is_last)
                add(arg, 1);
            count = 1;
            prev = arg;
        }
    }
#if !defined(NDEBUG)
    std::cout << " -> " << new_args << std::endl;
#endif
    return new_args;
}

std::vector<symcxx::idx_t>
symcxx::NameSpace::merge(const Kind kind, const std::vector<idx_t>& sorted_args) const {
#if !defined(NDEBUG)
    std::cout << "merge(kind=" << kind_names[static_cast<int>(kind)] << ", sorted_args="<< sorted_args <<")";
#endif
    std::vector<idx_t> new_args;
    bool merged = false;
    for (const auto idx : sorted_args){
        if (instances[idx].kind == kind){
            merged = true;
            for (const auto inner_idx : instances[idx].args_from_stack())
                new_args.push_back(inner_idx);
        }else{
            new_args.push_back(idx);
        }
    }
    if (merged)
        new_args = merge(kind, sort_args(new_args));
#if !defined(NDEBUG)
    std::cout << " -> " << new_args << std::endl;
#endif
    return new_args;
}

std::vector<symcxx::idx_t>
symcxx::NameSpace::sort_args(const std::vector<idx_t>& args) const {
#if !defined(NDEBUG)
    std::cout << "sort_args(args="<< args<<")";
#endif
    std::vector<idx_t> new_args(args);
    std::sort(new_args.begin(), new_args.end(),
              [&] (const idx_t a, const idx_t b){ return this->instances[a] < this->instances[b]; } );
#if !defined(NDEBUG)
    std::cout << " -> " << new_args << std::endl;
#endif
    return new_args;
}

std::vector<symcxx::idx_t>
symcxx::NameSpace::merge_drop_sort_collect(const std::vector<idx_t>& args,
                                           const Kind collect_to,
                                           const std::vector<idx_t>& drop,
                                           const Kind merge_kind) {
#if !defined(NDEBUG)
    std::cout << "merge_drop_sort_collect(args=" << args << ", collect_to=" << kind_names[static_cast<int>(collect_to)] <<
        ", drop=" << drop << ", merge_kind=" << kind_names[static_cast<int>(merge_kind)] << ")";
#endif
    std::vector<idx_t> new_args;
    for (const auto arg : merge(merge_kind, sort_args(args))){
        for (const auto drp : drop){
// #if !defined(NDEBUG)
//             std::cout << "Comparing arg, drp: " << arg << ", " << drp << std::endl;
// #endif
            if (arg == drp)
                goto break_twice;
        }
        new_args.push_back(arg);
    break_twice: ;
    }
    new_args = collect(collect_to, sort_args(new_args));
#if !defined(NDEBUG)
    std::cout << " -> " << new_args << std::endl;
#endif
    return new_args;
}
