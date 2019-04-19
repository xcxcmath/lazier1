//
// Created by 김 범주 on 2019-02-24.
//

#ifndef LAZIER1_VARIABLE_HPP
#define LAZIER1_VARIABLE_HPP

#include <lazier/Expression.hpp>

namespace lazier {
    template<typename T>
    class Variable : public Expression<T> {
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
        Initializer initializer;

        LAZIER_DELETE_SPECIAL_FUNCTIONS(Variable);

    protected:
        /*
         * protected constructors
         */
        explicit Variable() : Expression<T>(), initializer() {
            this->m_isOptimizerTarget = true;
            this->m_isResettableTensor = false;
        }
        explicit Variable(Initializer f) : Variable() {
            initializer = std::move(f);
            this->eval = [this](Session<T>& sess) -> T {

                /*
                 * When this eval function is called,
                 * the value related to this Variable is guaranteed to be reset.
                 */

                return initializer(sess);
            };
        }
        explicit Variable(T rhs) : Variable([rhs = std::move(rhs)](Session<T>&)->T{return rhs;}) {}

        /*
         * friends
         */

        template<typename T1, typename ...Types>
        friend typename Expression<T1>::SharedPtr make_variable(Types&& ...args);
    };

    template<typename T, typename ...Types>
    typename Expression<T>::SharedPtr make_variable(Types&& ...args) {
        return typename Expression<T>::SharedPtr(new Variable<T>(std::forward<Types>(args)...));
    }

}

#endif //LAZIER1_VARIABLE_HPP
