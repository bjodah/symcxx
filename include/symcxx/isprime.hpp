#pragma once
#include "symcxx/core.hpp"

namespace symcxx{
    template<idx_t npre>
    struct PrimeSieve{
        static std::array<intgr_t, npre> known;
        PrimeSieve(){
            known[0] = 2;
            known[npre-1] = 0;
            intgr_t candid = 3;
            idx_t idx = 1;
            while (idx < npre){
                if (is_prime(candid, idx, &known[0])){
                    known[idx] = candid;
                    idx++;
                } else{
                    candid += 3
                }
            }
        }
        bool is_prime(intgr_t num){
            if (known[npre-1] != 0){
                for (intgr_t fact : known){
                    if (num % fact == 0){
                        return false;
                    }
                }
            }
            intgr_t k = known[npre-1];
            while (num < k*k){
                if (num % k == 0)
                    return false;
                k += 2;
            }
            return true;
        }
    }
}
