/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>
#include <vector>
#include <inviwo/molecularchargetransitions/algorithm/statistics.h>

namespace inviwo {

TEST(MolecularChargeTransitions, Mean_VectorWithTwoElements_CalculatesCorrectly) {
    auto v = std::vector<float>{8.0f, 10.0f};
    auto mean = VectorStatistics::meanValue(v);

    EXPECT_FLOAT_EQ(9.0f, mean);
}

TEST(MolecularChargeTransitions, Variance_VectorWithTwoElements_CalculatesCorrectly) {
    auto v = std::vector<float>{8.0f, 10.0f};
    auto var = VectorStatistics::variance(v, 9.0f);

    EXPECT_FLOAT_EQ(1.0f, var);
}
TEST(MolecularChargeTransitions, Mean_VectorWithThreeElements_CalculatesCorrectly) {
    auto v = std::vector<float>{8.0f, 10.0f, 15.0f};
    auto mean = VectorStatistics::meanValue(v);

    EXPECT_FLOAT_EQ(11.0f, mean);
}

TEST(MolecularChargeTransitions, Variance_VectorWithThreeElements_CalculatesCorrectly) {
    auto v = std::vector<float>{8.0f, 10.0f, 15.0f};
    auto var = VectorStatistics::variance(v, 11.0f);

    EXPECT_FLOAT_EQ(8.66667f, var);
}

}  // namespace inviwo
