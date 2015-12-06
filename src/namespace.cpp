#include "symcxx/core.hpp"

symcxx::NameSpace::NameSpace(idx_t n_pre_symbs) : n_pre_symbs(n_pre_symbs), n_symbs(n_pre_symbs) {
    instances.reserve(2*n_pre_symbs + n_pre_intgrs);
    for (idx_t idx=0; idx < n_pre_symbs; ++idx)
        instances.push_back(Symbol(idx, this));
    for (idx_t idx=0; idx < n_pre_intgrs; ++idx)
        instances.push_back(Integer(idx, this));
#if !defined(NDEBUG)
    std::cout << "n_pre_intgrs=" << n_pre_intgrs << std::endl;
    for (int i=0; i<static_cast<int>(Kind::Kind_Count); ++i)
        std::cout << i << ": " << kind_names[i] << std::endl;
#endif
}

symcxx::idx_t
symcxx::NameSpace::idx(const Basic* basic_ptr) const {
    const auto begin = &instances[0];
    const auto end = begin + instances.capacity();
    if ((basic_ptr < begin) || (basic_ptr > end))
        throw std::runtime_error("Instance not in namespace.");
    return (basic_ptr - begin);
}

symcxx::idx_t
symcxx::NameSpace::reg_args(const std::vector<idx_t>& objs) {
    args_stack.push_back(objs);
    return args_stack.size() - 1;
}

bool
symcxx::NameSpace::has(const Basic& looking_for, idx_t * idx) const {
    idx_t idx_ = 0;
#if !defined(NDEBUG)
    std::cout << "has(looking_for.hash=" << looking_for.hash << ")";
#endif
    for (const auto& inst : instances){
        if (looking_for == inst){
            *idx = idx_;
#if !defined(NDEBUG)
            std::cout << " -> true";
#endif
            return true;
        }
        ++idx_;
    }
#if !defined(NDEBUG)
    std::cout << " -> false";
#endif
    return false;
}

bool
symcxx::NameSpace::is_zero(const idx_t idx) const {
    if ((instances[idx].kind == Kind::Float && instances[idx].data.dble == 0) ||
        (instances[idx].kind == Kind::Integer && instances[idx].data.intgr == 0))
        return true;
    else
        return false;
}

bool
symcxx::NameSpace::is_one(const idx_t idx) const {
    if ((instances[idx].kind == Kind::Float && instances[idx].data.dble == 1) ||
        (instances[idx].kind == Kind::Integer && instances[idx].data.intgr == 1))
        return true;
    else
        return false;
}

symcxx::idx_t
symcxx::NameSpace::make_symbol(idx_t symb_idx){
    if (symb_idx < n_pre_symbs)
        return symb_idx;
    const auto instance = Symbol(symb_idx, this);
    idx_t idx;
    if (has(instance, &idx)){
#if !defined(NDEBUG)
        std::cout << "make_symbol(" << symb_idx << ") - old!" << std::endl;
#endif
        throw std::runtime_error("Something fishy about that call..");
        return idx;
    }
#if !defined(NDEBUG)
    std::cout << "make_symbol(" << idx << ") - new!" << std::endl;
#endif
    if (symb_idx != n_symbs)
        throw std::runtime_error("Something fishy about skipping symbols..");
    n_symbs++;
    instances.push_back(instance);
    return instances.size() - 1;
}

symcxx::idx_t
symcxx::NameSpace::make_integer(int i){
    if (i >= 0 && static_cast<idx_t>(i) < n_pre_intgrs)
        return n_pre_symbs + i;
    const auto instance = Integer(i, this);
    idx_t idx;
    if (has(instance, &idx)){
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
    if (has(instance, &idx))
        return idx;
    instances.push_back(instance);
    return instances.size() - 1;
}
symcxx::idx_t
symcxx::NameSpace::make_nan(){
    return make_float(std::nan(""));
}

#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    symcxx::idx_t symcxx::NameSpace::METH_(const std::vector<symcxx::idx_t>& args){ \
        const auto instance = CLS_(reg_args(args), this);                \
        idx_t idx;                                                      \
        if (has(instance, &idx)){                                       \
            std::cout << STRINGIFY(METH_) "("<< args <<") - old!" << std::endl; \
            args_stack.pop_back();                                      \
            return idx;                                                 \
        }                                                               \
        std::cout << STRINGIFY(METH_) "("<< args <<") - new!" << std::endl;              \
        instances.push_back(instance);                                  \
        return instances.size() - 1;                                    \
    }
#include "symcxx/types_nonatomic_args_stack.inc"
#undef SYMCXX_TYPE


#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    symcxx::idx_t symcxx::NameSpace::METH_(const symcxx::idx_t inst){ \
        const auto instance = CLS_(inst, this);                          \
        idx_t idx;                                                      \
        if (has(instance, &idx)){                                       \
            std::cout << STRINGIFY(METH_) "("<< inst <<") - old!" << std::endl;          \
            return idx;                                                 \
        }                                                               \
        std::cout << STRINGIFY(METH_) "("<< inst <<") - new!" << std::endl;   \
        instances.push_back(instance);                                  \
        return instances.size() - 1;                                    \
    }
#include "symcxx/types_nonatomic_unary.inc"
#undef SYMCXX_TYPE

#define SYMCXX_TYPE(CLS_, PARENT_, METH_) \
    symcxx::idx_t symcxx::NameSpace::METH_(const symcxx::idx_t inst0, const symcxx::idx_t inst1){  \
        const auto instance = CLS_(inst0, inst1, this);                  \
        idx_t idx;                                                      \
        if (has(instance, &idx)){                                       \
            std::cout << STRINGIFY(METH_) "("<< inst0 <<", "<< inst1 <<") - old!" << std::endl;          \
            return idx;                                                 \
        }                                                               \
        std::cout << STRINGIFY(METH_) "("<< inst0 <<", "<< inst1 <<") - new!" << std::endl;              \
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
    switch(kind){
    case Kind::Add:
        if (args.size() == 0)
            throw std::runtime_error("create Add from length 0 vector of arguments");
        else if (args.size() == 1)
            return args[0];
        else if (args.size() == 2)
            return add2(args[0], args[1]);
        else
            return add(merge_drop_sort_collect(args, Kind::Mul, {zero, mul({zero})}, Kind::Add));
    case Kind::Mul:
        if (args.size() == 0)
            throw std::runtime_error("create Mul from length 0 vector of arguments");
        else if (args.size() == 1)
            return args[0];
        else if (args.size() == 2)
            return mul2(args[0], args[1]);
        else
            return mul(merge_drop_sort_collect(args, Kind::Pow, {one}, Kind::Mul));
    case Kind::ITE:
        if (args.size() != 3)
            throw std::runtime_error("create ITE from vector of length != 3");
        else if (args[1] == args[2])
            return args[1];
        else
            return ite(args);
    default:
#if !defined(NDEBUG)
        std::cout << "!create does not support kind:" << kind_names[static_cast<int>(kind)] << std::endl;
#endif
        throw std::runtime_error("create does not support kind.");
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
        if (inst_idx0 == inst_idx1)
            return create(Kind::Mul2, make_integer(2), inst_idx0);
        if (are_sorted())
            return add2(inst_idx0, inst_idx1);
        else
            return add2(inst_idx1, inst_idx0);
    case Kind::Mul:
    case Kind::Mul2:
        if (inst_idx0 == inst_idx1)
            return create(Kind::Pow, inst_idx0, make_integer(2));
        if (are_sorted())
            return mul2(inst_idx0, inst_idx1);
        else
            return mul2(inst_idx1, inst_idx0);
    case Kind::Sub:
        if (inst_idx0 == inst_idx1)
            return make_integer(0);
        else
            if (are_sorted())
                return sub(inst_idx0, inst_idx1);
            else
                return sub(inst_idx1, inst_idx0);
    case Kind::Div:
        if (is_zero(inst_idx1))
            return make_nan();
        else
            if (is_zero(inst_idx0))
                return make_integer(0);
            else
                return sub(inst_idx0, inst_idx1);
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
    case Kind::Hypot:
    default:
        throw std::runtime_error("create does not support kind.");
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
        for (const auto idx : inst.args_from_stack()){
            args.push_back(diff(idx, wrt_id));
        }
        return create(Kind::Add, args);
    case Kind::Add2:
        return create(Kind::Add,
                      diff(inst.data.idx_pair.first, wrt_id),
                      diff(inst.data.idx_pair.second, wrt_id));
    default:
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
            new_args.push_back(create(collect_to_kind, make_integer(cnt_), arg_));
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
        for (const auto drp : drop)
            if (arg == drp)
                continue;
        new_args.push_back(arg);
    }
    new_args = collect(collect_to, sort_args(new_args));
#if !defined(NDEBUG)
    std::cout << " -> " << new_args << std::endl;
#endif
    return new_args;
}
