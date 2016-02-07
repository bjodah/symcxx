#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    struct Matrix {
        const idx_t nr, nc;
        const std::vector<idx_t> data;
        Matrix(idx_t nr, idx_t nc, const std::vector<idx_t> data) :
            nr(nr), nc(nc), data(data) {
            if ((nr < 1) || (nc < 1) || (data.size() != nr*nc))
                throw std::runtime_error("improper dimensions");
        }
        Matrix jacobian(const Matrix& dx, NameSpace * const ns) const {
            if ((nr > 1 && nc > 1) || (dx.nr > 1 && dx.nc > 1))
                throw std::runtime_error("improper dimensions");
            const idx_t jnr = std::max(nr, nc);
            const idx_t jnc = std::max(dx.nr, dx.nc);
            std::vector<idx_t> new_data;
            new_data.resize(jnc*jnr);
            for (idx_t ri=0; ri<jnr; ++ri){
                for (idx_t ci=0; ci<jnc; ++ci){
                    new_data[ri*jnc + ci] = ns->diff(data[ri], dx.data[ci]);
                }
            }
            return Matrix(jnr, jnc, new_data);
        };
        void evalf_all(const double * const __restrict__ inp,
                       double * const __restrict__ out,
                       const NameSpace * const __restrict__ ns) const {
            for (idx_t ri=0; ri<nr; ++ri){
                for (idx_t ci=0; ci<nc; ++ci){
                    out[ri*nc + ci] = ns->evalf(data[ri*nc + ci], inp);
                }
            }
        }
    };
}
