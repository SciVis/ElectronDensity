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

#include <inviwo/molecularchargetransitions/processors/sumchargeinsegmentedregions.h>

namespace inviwo {

	// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
	const ProcessorInfo SumChargeInSegmentedRegions::processorInfo_{
		"org.inviwo.SumChargeInSegmentedRegions",      // Class identifier
		"Sum Charge In Segmented Regions",             // Display name
		"Undefined",								   // Category
		CodeState::Experimental,					   // Code state
		Tags::None,									   // Tags
	};
	const ProcessorInfo SumChargeInSegmentedRegions::getProcessorInfo() const { return processorInfo_; }

	SumChargeInSegmentedRegions::SumChargeInSegmentedRegions()
		: Processor()
		, segmentation_("volumeSegmentation")
		, volumeValues_("chargeDensity")
		, chargePerRegion_("chargePerRegion")
		, chargePerSubgroup_("chargePerSubgroup") {

		addPort(segmentation_);
		addPort(volumeValues_);
		addPort(chargePerRegion_);
		addPort(chargePerSubgroup_);
	}

	void SumChargeInSegmentedRegions::process() {
		// TODO: Should have the option to sum whole volume as well?

		const auto segmentationData = segmentation_.getData();
		const auto volumeData = volumeValues_.getData();

		if (volumeData->getDimensions() != segmentationData->getDimensions()) {
			throw Exception("Unexpected dimension missmatch", IVW_CONTEXT);
		}

		const auto range = segmentationData->dataMap_.valueRange;

		// TODO: do I really need to set an initial value here?
		auto indicesToAccumulatedValue = std::unordered_map<uint16_t, float>();
		for (auto i = static_cast<uint16_t>(range.x); i < static_cast<uint16_t>(range.y); i++) {
			indicesToAccumulatedValue.emplace(i, 0.0);
		}

		if (indicesToAccumulatedValue.size() == 0) {
			throw Exception("Seem to be no segmented regions in the segmented volume...", IVW_CONTEXT);
		}

		const util::IndexMapper3D im(volumeData->getDimensions());
		volumeData->getRepresentation<VolumeRAM>()->dispatch<void, dispatching::filter::FloatScalars>([&](auto vr) {
			using ChargeDensityValueType = util::PrecisionValueType<decltype(vr)>;
			const ChargeDensityValueType* src = vr->getDataTyped();

			segmentationData->getRepresentation<VolumeRAM>()->dispatch<void, dispatching::filter::UnsignedIntegerScalars>([&](auto seg) {
				using VolumeSegmentationValueType = util::PrecisionValueType<decltype(seg)>;
				const VolumeSegmentationValueType* indices = seg->getDataTyped();

				util::forEachVoxel(*vr, [&](const size3_t& pos) {
					float value = static_cast<float>(src[im(pos)]);
					uint16_t index = static_cast<uint16_t>(indices[im(pos)]);
					indicesToAccumulatedValue.at(index) = indicesToAccumulatedValue.at(index) + value;
					});
				});
			});

		float totalCharge =
			std::accumulate(indicesToAccumulatedValue.begin(), indicesToAccumulatedValue.end(), 0.0f,
				[](float value, auto current) { return value + current.second; });

		auto dataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(3 * indicesToAccumulatedValue.size()));
		auto& col1 = dataFrame->addColumn<uint16_t>("Segmented region", indicesToAccumulatedValue.size())
			->getTypedBuffer()
			->getEditableRAMRepresentation()
			->getDataContainer();
		auto& col2 = dataFrame->addColumn<float>("Charge", indicesToAccumulatedValue.size())
			->getTypedBuffer()
			->getEditableRAMRepresentation()
			->getDataContainer();
		auto& col3 = dataFrame->addColumn<float>("Charge [%]", indicesToAccumulatedValue.size())
			->getTypedBuffer()
			->getEditableRAMRepresentation()
			->getDataContainer();

		size_t i = 0;
		for (auto& indexToAccumulatedValue : indicesToAccumulatedValue) {
			col1[i] = indexToAccumulatedValue.first;
			col2[i] = indexToAccumulatedValue.second;
			col3[i] = 100.0 * indexToAccumulatedValue.second / totalCharge;
			i++;
		}

		// Subgroup information, should be an inport?
		auto subgroupInfo = std::unordered_map<uint16_t, std::vector<uint16_t>>();
		// Thio
		subgroupInfo.emplace(0, std::vector<uint16_t>{0, 1, 2, 3, 12, 13, 14, 15});
		// Quin
		subgroupInfo.emplace(1, std::vector<uint16_t>{4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 18, 19, 20, 21, 22});

		// TODO: check that the subgroups together contain all segmented regions

		auto subgroupDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(2 * subgroupInfo.size()));
		auto& colA = subgroupDataFrame->addColumn<uint16_t>("index_sg", subgroupInfo.size())
			->getTypedBuffer()
			->getEditableRAMRepresentation()
			->getDataContainer();
		auto& colB = subgroupDataFrame->addColumn<float>("charge_sg", subgroupInfo.size())
			->getTypedBuffer()
			->getEditableRAMRepresentation()
			->getDataContainer();

		size_t j = 0;
		for (auto& subgroup : subgroupInfo) {
			colA[j] = subgroup.first;
			colB[j] = std::accumulate(subgroup.second.begin(), subgroup.second.end(), 0.0f,
				[col3](float value, auto current) { return value + col3[current]; });
			j++;
		}

		chargePerRegion_.setData(dataFrame);
		chargePerSubgroup_.setData(subgroupDataFrame);
	}

}  // namespace inviwo
