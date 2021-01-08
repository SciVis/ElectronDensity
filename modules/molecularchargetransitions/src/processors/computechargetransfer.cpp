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
		"org.inviwo.ComputeChargeTransfer",      // Class identifier
		"Compute Charge Transfer",               // Display name
		"Undefined",							 // Category
		CodeState::Experimental,				 // Code state
		Tags::None,								 // Tags
	};
	const ProcessorInfo ComputeChargeTransfer::getProcessorInfo() const { return processorInfo_; }

	ComputeChargeTransfer::ComputeChargeTransfer()
		: Processor()
		, holeCharges_("holeCharges")
		, particleCharges_("particleCharges")
		, chargeDifference_("chargeDifference")
		, chargeTransfer_("chargeTransfer") {

		addPort(holeCharges_);
		addPort(particleCharges_);
		addPort(chargeDifference_);
		addPort(chargeTransfer_);
	}

	void ComputeChargeTransfer::process() {

		const auto holeCharges = holeCharges_.getData()->getColumn("charge_sg")
			->getBuffer()
			->getRepresentation<BufferRAM>()
			->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
			auto& data = buf->getDataContainer();
			std::vector<float> dst(data.size(), 0.0f);
			std::transform(data.begin(), data.end(), dst.begin(),
				[&](auto v) { return static_cast<float>(v); });
					return dst;
				});

		const auto particleCharges = particleCharges_.getData()->getColumn("charge_sg")
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

		const auto n = holeCharges.size();

		auto chargeDiffDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(n));
		auto& col1 = chargeDiffDataFrame->addColumn<float>("Charge difference", n)
			->getTypedBuffer()
			->getEditableRAMRepresentation()
			->getDataContainer();

		auto chargeTransferDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(n));

		// Charge transfer matrix 
		// (Actually the transpose of the "real" charge transfer matrix, 
		// to easily obtain the columns as rows below)
		std::vector<std::vector<float>> transfer = {};
		transfer.resize(n);

		auto donors = std::vector<size_t>();
		auto acceptors = std::vector<size_t>();
		auto chargeDifference = std::vector<float>();

		for (auto i = 0; i < n; i++) {
			const auto chargeDiff = particleCharges[i] - holeCharges[i];
			col1[i] = chargeDiff;
			chargeDifference.push_back(chargeDiff);

			// Donor
			if (chargeDiff < 0) {
				donors.push_back(i);
			}
			// Acceptor
			else {
				acceptors.push_back(i);
			}
			transfer[i].resize(n);
			transfer[i][i] = std::min(holeCharges[i], particleCharges[i]);
		}

		float totalAcceptorCharge = 0.0f;
		for (auto& aIndex : acceptors) {
			totalAcceptorCharge += chargeDifference[aIndex];
		}

		// Heuristic
		for (auto& dIndex : donors) {
			for (auto& aIndex : acceptors) {
				// transpose of charge transfer matrix
				transfer[dIndex][aIndex] = -chargeDifference[dIndex] * 
					(chargeDifference[aIndex] / totalAcceptorCharge);
			}
		}

		for (auto i = 0; i < n; i++) {
			auto& col = chargeTransferDataFrame->addColumn<float>(toString(i + 1), n)
				->getTypedBuffer()
				->getEditableRAMRepresentation()
				->getDataContainer();
			// since this matrix is the transpose of the charge transfer matrix, 
			// the column is obtained by taking the row
			col = transfer[i];
		}

		chargeDifference_.setData(chargeDiffDataFrame);
		chargeTransfer_.setData(chargeTransferDataFrame);
	}

}  // namespace inviwo
