//
// Created by bjk on 19. 2. 21.
//

#ifndef LAZIER1_SESSION_HPP
#define LAZIER1_SESSION_HPP

#include <lazier/lazierCommon.hpp>

namespace lazier {

    template<typename T>
    class Expression;

    template<typename T>
    class Session {
    public:
        friend class Expression<T>;

        /*
         * Type definitions
         */
        using ExpPointer = std::shared_ptr<Expression<T>>;
        using FeedMap = std::map<ExpPointer, T>;

        /*
         * member functions
         */

        const T& run(const ExpPointer& target, const FeedMap& feed_map = {}) {
            feedMap(feed_map);
            return eval(target);
        }

        const T& eval(const ExpPointer& target) {
            if(auto it = m_eval.find(target); it != m_eval.end()){
                return m_eval[target];
            }

            return m_eval[target] = target->eval(*this);
        }

        const T& diff(const ExpPointer& target, const ExpPointer& E) {
            if(auto out_it = m_diff.find(E); out_it != m_diff.end()) {
                if(auto in_it = out_it->second.find(target); in_it != out_it->second.end()) {
                    return m_diff[E][target];
                }
            }

            auto& cache = m_diff[E][target];
            cache = xt::zeros_like(eval(target));

            if(E == target) {
                cache = xt::ones_like(eval(target));
                return cache;
            }

            for(const auto &[ptr, derivative] : target->diff) {
                cache += derivative(*this, E);
            }

            return cache;
        }

        void feedMap(const FeedMap& feed_map) {
            for(const auto&[ptr, val] : feed_map) {
                ptr->resetTensor(*this);
                m_eval[ptr] = val;
            }
        }

    protected:
        /*
         * protected member variables
         */
        std::map<ExpPointer, T> m_eval;
        std::map<ExpPointer, std::map<ExpPointer, T>> m_diff;
    };

}

#endif //LAZIER1_SESSION_HPP
