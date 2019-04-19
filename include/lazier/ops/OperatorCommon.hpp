//
// Created by 김 범주 on 2019-03-06.
//

#ifndef LAZIER1_OPERATORCOMMON_HPP
#define LAZIER1_OPERATORCOMMON_HPP

#include <lazier/Expression.hpp>

namespace lazier::ops {

    /*
     * extract what axis are for broadcasting
     */
    template<typename S>
    typename Tensor<S>::shape_type broadcast_axis
            (const Tensor<S> &from, const Tensor<S> &target) {
        auto from_shape = from.shape();
        auto from_dim = static_cast<long>(from_shape.size());

        auto target_shape = target.shape();
        auto target_dim = static_cast<long>(target_shape.size());

        typename Tensor<S>::shape_type ret;

        auto remain_dim = target_dim - from_dim;

        if(remain_dim < 0)
            return ret;

        for(long i = 0; i < remain_dim; ++i) {
            ret.push_back(i);
        }

        for(long i = remain_dim; i < target_dim; ++i) {
            if(from_shape[i - remain_dim] < target_shape[i]) {
                ret.push_back(i);
            }
        }

        return ret;
    }

    /*
     * shape that keeps dimension of [target] when it's reduced on [axis]
     */

    template<typename S>
    xt::xstrided_slice_vector keep_dims_vector
            (const Tensor<S> &target, const std::vector<Index> &axis) {
        auto target_shape = target.shape();
        std::size_t target_dim = target_shape.size();

        xt::xstrided_slice_vector ret(target_dim, xt::all());

        for(auto i : axis) {
            ret[i] = xt::newaxis();
        }

        return ret;
    }

}

#endif //LAZIER1_OPERATORCOMMON_HPP
