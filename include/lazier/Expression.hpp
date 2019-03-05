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
        // value type
        using TensorType = T;

        // smart pointer types & set of them
        using SharedPtr = std::shared_ptr<Expression<T>>;
        using SharedPtrSet = std::set<SharedPtr>;
        using WeakPtr = std::weak_ptr<Expression<T>>;
        using WeakPtrSet = std::set<WeakPtr, std::owner_less<WeakPtr>>;

        // value-delta-evaluating function
        using Initializer = std::function<T(Session<T>&)>;
        using Evaluator = std::function<T(Session<T>&)>;
        using Differentiator = std::function<T(Session<T>&, const SharedPtr&)>;

        /*
         * member variables
         */

        Evaluator eval;
        std::map<WeakPtr, Differentiator, std::owner_less<WeakPtr>> diff;
        WeakPtrSet pre;
        WeakPtrSet post;

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
            auto this_ptr = getPointer();

            if(auto it = sess.eval_cache.find(this_ptr); it != sess.eval_cache.end()) {
                if(m_isResettableTensor) {
                    sess.eval_cache.erase(it);
                }
                for(const auto &post_ptr : post) {
                    if(auto p = post_ptr.lock()) {
                        p->resetTensor(sess);
                    }
                }
            }

            if(post.empty()){
                resetDelta(sess);
            }
        }

        virtual void resetDelta(Session<T>& sess) {
            auto this_ptr = getPointer();

            for(auto &[E, mp] : sess.diff_cache) {
                if(auto it = mp.find(this_ptr); it != mp.end()) {
                    mp.erase(it);
                    for(const auto &pre_ptr : pre) {
                        if(auto p = pre_ptr.lock()) {
                            p->resetDelta(sess);
                        }
                    }
                }
            }

        }

        bool isOptimizerTarget() const {
            return m_isOptimizerTarget;
        }

    protected:
        /*
         * protected member variables
         */
        bool m_isOptimizerTarget;
        bool m_isResettableTensor;

        /*
         * protected constructors
         */
        explicit Expression()
        : eval(), diff(), pre(), post(),
        m_isOptimizerTarget(false), m_isResettableTensor(true) {}

        /*
         * friends
         */

        template<typename T1, typename ...Types>
        friend typename Expression<T1>::SharedPtr make_expression(Types&& ...args);

        template<typename T1>
        friend void relate_expression
        (const typename Expression<T1>::SharedPtrSet& preExp, const typename Expression<T1>::SharedPtrSet& postExp);
    };

    template<typename T, typename ...Types>
    typename Expression<T>::SharedPtr make_expression(Types&& ...args) {
        return typename Expression<T>::SharedPtr(new Expression<T>(std::forward<Types>(args)...));
    }

    template<typename T>
    void relate_expression
    (const typename Expression<T>::SharedPtrSet& preExp, const typename Expression<T>::SharedPtrSet& postExp) {
        for(const auto& ptr : preExp) {
            ptr->post.insert(postExp.cbegin(), postExp.cend());
        }

        for(const auto& ptr : postExp) {
            ptr->pre.insert(preExp.cbegin(), preExp.cend());
        }
    }

}

#endif //LAZIER1_EXPRESSION_HPP
