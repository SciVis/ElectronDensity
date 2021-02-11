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
#include <inviwo/molecularchargetransitions/algorithm/chargetransfermatrix.h>
#include <inviwo/core/util/exception.h>

namespace inviwo {

void checkChargeTransferAndChargeDifference(
    const std::vector<std::vector<float>>& chargeTransfer,
    const std::vector<float>& chargeDifference,
    const std::vector<std::vector<float>>& expectedChargeTransfer,
    const std::vector<float>& expectedChargeDifference) {
    EXPECT_TRUE(chargeTransfer.size() == expectedChargeTransfer.size());
    for (size_t i = 0; i < expectedChargeTransfer.size(); i++) {
        EXPECT_TRUE(chargeTransfer[i].size() == expectedChargeTransfer[i].size());
        for (size_t j = 0; j < chargeTransfer[i].size(); j++) {
            EXPECT_FLOAT_EQ(chargeTransfer[i][j], expectedChargeTransfer[i][j]);
        }
    }

    EXPECT_TRUE(chargeDifference.size() == expectedChargeDifference.size());
    for (size_t i = 0; i < expectedChargeDifference.size(); i++) {
        EXPECT_FLOAT_EQ(chargeDifference[i], expectedChargeDifference[i]);
    }
}

TEST(MolecularChargeTransitions,
     ComputeTransposedChargeTransferAndChargeDifference_TwoSubgroupsOneDonor_ReturnsExpected) {
    const auto holeCharges = std::vector<float>{0.5f, 0.5f};
    const auto particleCharges = std::vector<float>{1.0f, 0.0f};
    const auto [chargeTransfer, chargeDifference] =
        ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(holeCharges,
                                                                                 particleCharges);
    checkChargeTransferAndChargeDifference(
        chargeTransfer, chargeDifference,
        /*expectedChargeTransfer*/
        std::vector<std::vector<float>>{{0.5f, 0.0f}, {0.5f, 0.0f}},
        /*expectedChargeDifference*/ std::vector<float>{0.5f, -0.5f});
}

TEST(MolecularChargeTransitions,
     ComputeTransposedChargeTransferAndChargeDifference_ThreeSubgroupsOneDonor_ReturnsExpected) {
    const auto holeCharges = std::vector<float>{0.5f, 0.2f, 0.3f};
    const auto particleCharges = std::vector<float>{0.1f, 0.4f, 0.5f};
    const auto [chargeTransfer, chargeDifference] =
        ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(holeCharges,
                                                                                 particleCharges);
    checkChargeTransferAndChargeDifference(
        chargeTransfer, chargeDifference,
        /*expectedChargeTransfer*/
        std::vector<std::vector<float>>{{0.1f, 0.2f, 0.2f}, {0.0f, 0.2f, 0.0f}, {0.0f, 0.0f, 0.3f}},
        /*expectedChargeDifference*/ std::vector<float>{-0.4f, 0.2f, 0.2f});
}

TEST(MolecularChargeTransitions,
     ComputeTransposedChargeTransferAndChargeDifference_ThreeSubgroupsTwoDonors_ReturnsExpected) {
    const auto holeCharges = std::vector<float>{0.5f, 0.2f, 0.3f};
    const auto particleCharges = std::vector<float>{0.1f, 0.7f, 0.2f};
    const auto [chargeTransfer, chargeDifference] =
        ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(holeCharges,
                                                                                 particleCharges);
    checkChargeTransferAndChargeDifference(
        chargeTransfer, chargeDifference,
        /*expectedChargeTransfer*/
        std::vector<std::vector<float>>{{0.1f, 0.4f, 0.0f}, {0.0f, 0.2f, 0.0f}, {0.0f, 0.1f, 0.2f}},
        /*expectedChargeDifference*/ std::vector<float>{-0.4f, 0.5f, -0.1f});
}

TEST(MolecularChargeTransitions,
     ComputeTransposedChargeTransferAndChargeDifference_EmptyHoleCharges_ThrowsException) {
    EXPECT_THROW(
        ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(
            /*holeCharges*/ std::vector<float>{}, /*particleCharges*/ std::vector<float>{1.0f}),
        inviwo::Exception);
}

TEST(MolecularChargeTransitions,
     ComputeTransposedChargeTransferAndChargeDifference_EmptyParticleCharges_ThrowsException) {
    EXPECT_THROW(
        ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(
            /*holeCharges*/ std::vector<float>{1.0f}, /*particleCharges*/ std::vector<float>{}),
        inviwo::Exception);
}

TEST(
    MolecularChargeTransitions,
    ComputeTransposedChargeTransferAndChargeDifference_ParticleAndHoleChargesNotSameSize_ThrowsException) {
    EXPECT_THROW(ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(
                     /*holeCharges*/ std::vector<float>{1.0f, 0.0f},
                     /*particleCharges*/ std::vector<float>{0.2f, 0.3f, 0.5f}),
                 inviwo::Exception);
}
}  // namespace inviwo
