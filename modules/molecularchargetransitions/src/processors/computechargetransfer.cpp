/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/molecularchargetransitions/processors/computechargetransfer.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ComputeChargeTransfer::processorInfo_{
    "org.inviwo.ComputeChargeTransfer",  // Class identifier
    "Compute Charge Transfer",           // Display name
    "Undefined",                         // Category
    CodeState::Experimental,             // Code state
    Tags::None,                          // Tags
};
const ProcessorInfo ComputeChargeTransfer::getProcessorInfo() const { return processorInfo_; }

ComputeChargeTransfer::ComputeChargeTransfer()
    : Processor()
    , holeCharges_("holeCharges")
    , particleCharges_("particleCharges")
    , chargeDifference_("chargeDifference")
    , chargeTransfer_("chargeTransfer")
    , holeAndParticleCharges_("holeAndParticleCharges") {

    addPort(holeCharges_);
    addPort(particleCharges_);
    addPort(chargeDifference_);
    addPort(chargeTransfer_);
    addPort(holeAndParticleCharges_);
}

void ComputeChargeTransfer::process() {

    const auto holeCharges =
        holeCharges_.getData()
            ->getColumn("charge_sg")
            ->getBuffer()
            ->getRepresentation<BufferRAM>()
            ->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
                auto& data = buf->getDataContainer();
                std::vector<float> dst(data.size(), 0.0f);
                std::transform(data.begin(), data.end(), dst.begin(),
                               [&](auto v) { return static_cast<float>(v); });
                return dst;
            });

    const auto particleCharges =
        particleCharges_.getData()
            ->getColumn("charge_sg")
            ->getBuffer()
            ->getRepresentation<BufferRAM>()
            ->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
                auto& data = buf->getDataContainer();
                std::vector<float> dst(data.size(), 0.0f);
                std::transform(data.begin(), data.end(), dst.begin(),
                               [&](auto v) { return static_cast<float>(v); });
                return dst;
            });

    if (holeCharges.size() != particleCharges.size()) {
        throw Exception("Unexpected dimension missmatch", IVW_CONTEXT);
    }
    if (holeCharges.size() == 0) {
        throw Exception("No input charges", IVW_CONTEXT);
    }

    const auto [chargeTransfer, chargeDifference] =
        ChargeTransferMatrix::computeTransposedChargeTransferAndChargeDifference(holeCharges,
                                                                                 particleCharges);

    const auto n = holeCharges.size();

    auto chargeDiffDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(n));
    auto& col1 = chargeDiffDataFrame->addColumn<float>("Charge difference", n)
                     ->getTypedBuffer()
                     ->getEditableRAMRepresentation()
                     ->getDataContainer();

    auto chargeTransferDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(n));
    for (auto i = 0; i < n; i++) {
        col1[i] = chargeDifference[i];
        auto& col = chargeTransferDataFrame->addColumn<float>(toString(i + 1), n)
                        ->getTypedBuffer()
                        ->getEditableRAMRepresentation()
                        ->getDataContainer();
        // This charge transfer matrix is the on the form "vector of columns"
        col = chargeTransfer[i];
    }

    std::vector<float> dst;
    std::merge(holeCharges.begin(), holeCharges.end(), particleCharges.begin(),
               particleCharges.end(), std::back_inserter(dst));
    auto holeAndParticleChargesDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(n));
    holeAndParticleChargesDataFrame->addColumn("charges", dst);

    chargeDifference_.setData(chargeDiffDataFrame);
    chargeTransfer_.setData(chargeTransferDataFrame);
    holeAndParticleCharges_.setData(holeAndParticleChargesDataFrame);
}

void ComputeChargeTransfer::doIfNotReady() {
    chargeDifference_.setData(nullptr);
    chargeTransfer_.setData(nullptr);
    holeAndParticleCharges_.setData(nullptr);
}

}  // namespace inviwo
