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
        /*
         * Type definitions
         */
        using ExpPointer = std::shared_ptr<Expression<T>>;
        using FeedMap = std::map<ExpPointer, T>;

        /*
         * member variables
         */
        std::map<ExpPointer, T> eval_cache;
        std::map<ExpPointer, std::map<ExpPointer, T>> diff_cache;

        /*
         * member functions
         */

        bool isEvalCached(const ExpPointer& target) {
            return eval_cache.find(target) != eval_cache.end();
        }

        const T& run(const ExpPointer& target, const FeedMap& feed_map = {}) {
            feedMap(feed_map);
            return eval(target);
        }

        const T& eval(const ExpPointer& target) {
            if(auto it = eval_cache.find(target); it != eval_cache.end()){
                return eval_cache[target];
            }

            return eval_cache[target] = target->eval(*this);
        }

        const T& diff(const ExpPointer& target, const ExpPointer& E) {
            if(auto out_it = diff_cache.find(E); out_it != diff_cache.end()) {
                if(auto in_it = out_it->second.find(target); in_it != out_it->second.end()) {
                    return diff_cache[E][target];
                }
            }

            auto& cache = diff_cache[E][target];
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
                eval_cache[ptr] = val;
            }
        }
    };

}

#endif //LAZIER1_SESSION_HPP
