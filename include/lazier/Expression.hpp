//
// Created by bjk on 19. 2. 21.
//

#ifndef LAZIER1_EXPRESSION_HPP
#define LAZIER1_EXPRESSION_HPP

#include <lazier/Session.hpp>

namespace lazier {

#define LAZIER_DELETE_SPECIAL_FUNCTIONS(Exp) \
Exp(const Exp&) = delete; \
Exp& operator=(const Exp&) = delete; \
Exp(Exp&&) noexcept = delete; \
Exp& operator=(Exp&&) noexcept = delete;

    template<typename S>
    using Tensor = xt::xarray<S>;

    using Index = std::size_t;

    template<typename S>
    using Shape = typename Tensor<S>::shape_type;

    template<typename T>
    class Expression : public std::enable_shared_from_this<Expression<T>> {
    public:
        /*
         * Type definitions
         */
        using TensorType = T;
        using SharedPtr = std::shared_ptr<Expression<T>>;
        using WeakPtr = std::weak_ptr<Expression<T>>;

        using Evaluator = std::function<T(Session<T>&)>;
        using Differentiator = std::function<T(Session<T>&, const SharedPtr&)>;

        /*
         * member variables
         */

        Evaluator eval;
        std::map<WeakPtr, Differentiator, std::owner_less<WeakPtr>> diff;
        std::set<WeakPtr, std::owner_less<WeakPtr>> pre;
        std::set<WeakPtr, std::owner_less<WeakPtr>> post;

        /*
         * member functions
         */

        LAZIER_DELETE_SPECIAL_FUNCTIONS(Expression);

        SharedPtr getPointer() {
            return this->shared_from_this();
        }

        WeakPtr getWeakPointer() {
            return this->weak_from_this();
        }

        virtual void resetTensor(Session<T>& sess) {
            if(sess.m_eval.count(getPointer()) != 0) {
                sess.m_eval.erase(getPointer());
                for(const auto &ptr : post) {
                    if(!ptr.expired()) {
                        ptr.lock()->resetTensor(sess);
                    }
                }
            }

            if(post.empty()){
                resetDelta(sess);
            }
        }

        virtual void resetDelta(Session<T>& sess) {
            for(auto &[E, mp] : sess.m_diff) {
                if(mp.count(getPointer()) != 0) {
                    mp.erase(getPointer());
                    for(const auto &ptr : pre) {
                        if(!ptr.expired()) {
                            ptr.lock()->resetDelta(sess);
                        }
                    }
                }
            }
        }

    protected:
        explicit Expression() : eval(), diff(), pre(), post() {}

        template<typename T1, typename ...Types>
        friend std::shared_ptr<Expression<T1>> make_expression(Types&& ...args);
    };

    template<typename T>
    using ExpPointer = std::shared_ptr<Expression<T>>;

    template<typename T, typename ...Types>
    ExpPointer<T> make_expression(Types&& ...args) {
        return ExpPointer<T>(new Expression<T>(std::forward<Types>(args)...));
    }

}

#endif //LAZIER1_EXPRESSION_HPP
