#pragma once

namespace symcxx{
    template<idx_t npre=512>
    struct PrimeSieve{
        std::array<intgr_t, npre> known;
        PrimeSieve(){
            known[0] = 2;
            known[npre-1] = 0;
            intgr_t candid = 3;
            idx_t idx = 1;
            while (idx < npre){
                if (is_prime(candid)){
                    known[idx] = candid;
                    idx++;
                }
                candid += 2;
            }
        }
        bool is_prime(intgr_t num) const{
            if (num == 1) {
                return true;
            }
            intgr_t k;
            if (known[npre-1] != 0){
                for (idx_t idx=0; idx < npre; ++idx){
                    k = known[idx];
                    if (k > num){
                        break;
                    }
                    if (num == k){
                        return true;
                    }
                    if (num % k == 0){
                        return false;
                    }
                }
            } else {
                if (num % 2 == 0){
                    return false;
                }
                k = 1;
            }

            while (num > k*k){
                k += 2;
                if (num == k)
                    return true;
                if (num % k == 0)
                    return false;
            }
            return true;
        }
    };
}
