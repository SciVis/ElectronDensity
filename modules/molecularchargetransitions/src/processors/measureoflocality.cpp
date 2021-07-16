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

#include <inviwo/molecularchargetransitions/processors/measureoflocality.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MeasureOfLocality::processorInfo_{
    "org.inviwo.MeasureOfLocality",  // Class identifier
    "Measure Of Locality",           // Display name
    "Undefined",                     // Category
    CodeState::Experimental,         // Code state
    Tags::None,                      // Tags
};
const ProcessorInfo MeasureOfLocality::getProcessorInfo() const { return processorInfo_; }

MeasureOfLocality::MeasureOfLocality()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , nrSubgroups_("nrSubgroups", "Nr of subgroups", 2, 1, 10, 1) {

    addPort(inport_);
    addPort(outport_);
    addProperty(nrSubgroups_);
}

void MeasureOfLocality::process() {
    auto iCol = inport_.getData()->getIndexColumn();
    auto& indexCol = iCol->getTypedBuffer()->getRAMRepresentation()->getDataContainer();

    std::vector<float> traces(indexCol.size(), 0.0f);

    std::vector<std::vector<float>> diagonalElements = {};
    const auto nrSubgroups = nrSubgroups_.get();
    for (size_t i = 0; i < nrSubgroups; i++) {
        auto diagonalElementsMatrixColumnName =
            "Charge transfer " + std::to_string(i + 1) + std::to_string(i + 1);

        auto diagonalElementsCol = inport_.getData()->getColumn(diagonalElementsMatrixColumnName);

        if (diagonalElementsCol == nullptr) {
            throw Exception(
                "Could not get charge transfer matrix diagonal column, subgroup " + std::to_string(i + 1),
                IVW_CONTEXT);
        }

        diagonalElements.push_back(
            diagonalElementsCol->getBuffer()
                ->getRepresentation<BufferRAM>()
                ->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
                    auto& data = buf->getDataContainer();
                    std::vector<float> dst(data.size(), 0.0f);
                    std::transform(data.begin(), data.end(), dst.begin(),
                                   [&](auto v) { return static_cast<float>(v); });
                    return dst;
                }));

        for (size_t j = 0; j < diagonalElements[i].size(); j++) {
            traces[j] += diagonalElements[i][j];
        }
    }

    auto dataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(traces.size()));
    dataFrame->addColumn("Measure of locality", traces);

    outport_.setData(dataFrame);
}

}  // namespace inviwo
