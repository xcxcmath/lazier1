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

    Expression<TF>::WeakPtr wa = a, wb = b, wc = c;

    c->eval = [wa, wb](Session<TF>& sess) -> TF {
        return sess.eval(wa.lock()) + sess.eval(wb.lock());
    };

    Session<TF> sess;

    TF ones = xt::ones<TF>({1});
    TF output = sess.run(c, {{a, ones}, {b, ones}});

    EXPECT_PRED2(tensorEqual<float>, ones + ones, output);
}