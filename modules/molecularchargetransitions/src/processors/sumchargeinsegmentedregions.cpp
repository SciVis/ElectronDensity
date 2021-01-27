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
		, chargePerSubgroup_("chargePerSubgroup")
		, fileLocation_("fileLocation", "Subgroup file location (json)") {

		addPort(segmentation_);
		addPort(volumeValues_);
		addPort(chargePerRegion_);
		addPort(chargePerSubgroup_);
		addProperty(fileLocation_);
	}

	void SumChargeInSegmentedRegions::process() {
		// TODO: Should have the option to sum whole volume as well?

		const auto segmentationData = segmentation_.getData();
		const auto volumeData = volumeValues_.getData();
		const auto fileLoc = fileLocation_.get();

		if (volumeData->getDimensions() != segmentationData->getDimensions()) {
			throw Exception("Unexpected dimension missmatch", IVW_CONTEXT);
			chargePerRegion_.setData(nullptr);
			chargePerSubgroup_.setData(nullptr);
		}
		else if (fileLoc == "") {
			throw Exception("No subgroup file provided", IVW_CONTEXT);
			chargePerRegion_.setData(nullptr);
			chargePerSubgroup_.setData(nullptr);
		}
		else if (!filesystem::fileExists(fileLoc)) {
			throw Exception("Subgroup file does not exist", IVW_CONTEXT);
			chargePerRegion_.setData(nullptr);
			chargePerSubgroup_.setData(nullptr);
		}
		else {
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
				col3[i] = indexToAccumulatedValue.second / totalCharge;
				i++;
			}

			auto fileStream = filesystem::ifstream(fileLoc);
			nlohmann::json subgroupsJson;
			fileStream >> subgroupsJson;

			const auto totalNrOfSubgroups = std::accumulate(subgroupsJson.cbegin(), subgroupsJson.cend(), 0,
				[&](uint16_t value, auto current) {
					if (!current.contains("indices")) {
						throw Exception("Wrong format on json object (does not contain 'indices')", IVW_CONTEXT);
					}
					return value + current["indices"].size();
				});

			if (totalNrOfSubgroups != indicesToAccumulatedValue.size()) {
				throw Exception("Subgroup info (indices) does not match the number of segmented regions", IVW_CONTEXT);
			}

			auto subgroupDataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(2 * subgroupsJson.size()));

			std::vector<std::string> subgroupNames = {};
			std::vector<float> charges = {};
			for (auto& subgroup : subgroupsJson) {
				if (!subgroup.contains("name")) {
					throw Exception("Wrong format on json object (does not contain 'name')", IVW_CONTEXT);
				}

				subgroupNames.push_back(subgroup["name"]);
				charges.push_back(std::accumulate(subgroup["indices"].begin(), subgroup["indices"].end(), 0.0f,
					[col3](float value, auto current) { return value + col3[current]; }));
			}
			subgroupDataFrame->addCategoricalColumn("subgroup", subgroupNames);
			subgroupDataFrame->addColumn("charge_sg", charges);

			chargePerRegion_.setData(dataFrame);
			chargePerSubgroup_.setData(subgroupDataFrame);
		}
	}

}  // namespace inviwo
