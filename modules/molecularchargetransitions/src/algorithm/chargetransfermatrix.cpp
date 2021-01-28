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

#include <inviwo/molecularchargetransitions/algorithm/chargetransfermatrix.h>

namespace inviwo {

std::pair<std::vector<std::vector<float>>, std::vector<float>>
ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(
    std::vector<float> holeCharges, std::vector<float> particleCharges) {
    const auto n = holeCharges.size();

    std::vector<std::vector<float>> chargeTransfer = {};
    chargeTransfer.resize(n);

    auto donors = std::vector<size_t>();
    auto acceptors = std::vector<size_t>();
    auto chargeDifference = std::vector<float>();

    for (auto i = 0; i < n; i++) {
        const auto chargeDiff = particleCharges[i] - holeCharges[i];
        chargeDifference.push_back(chargeDiff);

        // Donor
        if (chargeDiff < 0) {
            donors.push_back(i);
        }
        // Acceptor
        else {
            acceptors.push_back(i);
        }
        chargeTransfer[i].resize(n);
        chargeTransfer[i][i] = std::min(holeCharges[i], particleCharges[i]);
    }

    float totalAcceptorCharge = 0.0f;
    for (auto& aIndex : acceptors) {
        totalAcceptorCharge += chargeDifference[aIndex];
    }

    // Heuristic
    for (auto& dIndex : donors) {
        for (auto& aIndex : acceptors) {
            // transpose of charge transfer matrix
            chargeTransfer[dIndex][aIndex] =
                -chargeDifference[dIndex] * (chargeDifference[aIndex] / totalAcceptorCharge);
        }
    }

    return std::pair<std::vector<std::vector<float>>, std::vector<float>>{chargeTransfer,
                                                                          chargeDifference};
}

}  // namespace inviwo
