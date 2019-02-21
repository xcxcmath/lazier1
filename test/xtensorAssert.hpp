//
// Created by bjk on 19. 2. 22.
//

#ifndef LAZIER1_XTENSORASSERT_HPP
#define LAZIER1_XTENSORASSERT_HPP

#include <lazier/lazier>

template<typename S>
bool tensorNear
(const lazier::Tensor<S>& a, const lazier::Tensor<S>& b, S eps) {
    auto c = a - b;
    auto abs_eps = std::abs(eps);

    for(S elem : c) {
        if(elem > abs_eps || elem < -abs_eps)
            return false;
    }

    return true;
}

template<typename S>
bool tensorEqual
        (const lazier::Tensor<S>& a, const lazier::Tensor<S>& b) {
    return tensorNear<S>(a, b, 0);
}

#endif //LAZIER1_XTENSORASSERT_HPP
