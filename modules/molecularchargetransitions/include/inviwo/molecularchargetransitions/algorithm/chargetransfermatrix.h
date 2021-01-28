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
#pragma once

#include <inviwo/molecularchargetransitions/molecularchargetransitionsmoduledefine.h>
#include <vector>

namespace inviwo {

/**
 * Implementation to calculate the ("transposed") Charge Transfer matrix
 * and the charge difference (from hole to particle charge).
 *
 * The charge transfer matrix is on a "vector of columns" form (vector<vector<float>>)
 * to make it easier to obtain the columns (the "transpose" of the actual charge transfer matrix).
 *
 *     * holeCharges is a vector with the charges for the hole state.
 *     * particleCharges is a vector with the charges for the particle state.
 *
 * The input vectors must have the same length!
 */
class IVW_MODULE_MOLECULARCHARGETRANSITIONS_API ChargeTransferMatrix {
public:
    static std::pair<std::vector<std::vector<float>>, std::vector<float>>
    computeTransposedChargeTransferAndChargeDifference(std::vector<float> holeCharges,
                                                       std::vector<float> particleCharges);
};

}  // namespace inviwo
