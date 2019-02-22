//
// Created by bjk on 19. 2. 22.
//

#include "gtest/gtest.h"

#include "xtensorAssert.hpp"

using namespace lazier;

TEST(ClassExpression, basics) {
    using TF = Tensor<float>;

    auto a = make_expression<TF>();
    auto b = make_expression<TF>();
    auto c = make_expression<TF>();

    relate_expression<TF>({a, b}, {c});

    Expression<TF>::WeakPtr wa = a, wb = b, wc = c;

    c->eval = [wa, wb](Session<TF>& sess) -> TF {
        return sess.eval(wa.lock()) * sess.eval(wb.lock());
    };
    a->diff[wc] = [wb, wc](Session<TF>& sess, const Expression<TF>::SharedPtr& E) -> TF {
        return sess.diff(wc.lock(), E) * sess.eval(wb.lock());
    };
    b->diff[wc] = [wa, wc](Session<TF>& sess, const Expression<TF>::SharedPtr& E) -> TF {
        return sess.diff(wc.lock(), E) * sess.eval(wa.lock());
    };

    Session<TF> sess;

    TF operand1 = {5};
    TF operand2 = {3};

    TF output = sess.run(c, {{a, operand1}, {b, operand2}});
    TF delta_a = sess.diff(a, c);
    EXPECT_PRED2(tensorEqual<float>, operand1 * operand2, output);
    EXPECT_PRED2(tensorEqual<float>, operand2, delta_a);

    operand2 = {8};
    output = sess.run(c, {{a, operand1}, {b, operand2}});
    delta_a = sess.diff(a, c);
    EXPECT_PRED2(tensorEqual<float>, operand1 * operand2, output);
    EXPECT_PRED2(tensorEqual<float>, operand2, delta_a);
}