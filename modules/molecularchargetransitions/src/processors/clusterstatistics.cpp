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

#include <inviwo/molecularchargetransitions/processors/clusterstatistics.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ClusterStatistics::processorInfo_{
    "org.inviwo.ClusterStatistics",  // Class identifier
    "Cluster Statistics",            // Display name
    "Undefined",                     // Category
    CodeState::Experimental,         // Code state
    Tags::None,                      // Tags
};
const ProcessorInfo& ClusterStatistics::getProcessorInfo() const { return processorInfo_; }

ClusterStatistics::ClusterStatistics()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , diffOutport_("diffOutport")
    , meanOutport_("meanOutport")
    , meanMeasureOfLocalityOutport_("meanMeasureOfLocalityOutport")
    , nrSubgroups_("nrSubgroups", "Nr of subgroups", 2, 1, 10, 1)
    , clusterCol_{"clusterCol", "Column", inport_, ColumnOptionProperty::AddNoneOption::No, 0}
    , measureOfLocalityCol_{"measureOfLocalityCol", "Measure of Locality column", inport_,
                            ColumnOptionProperty::AddNoneOption::No, 0} {

    addPort(inport_);
    addPort(outport_);
    addPort(diffOutport_);
    addPort(meanOutport_);
    addPort(meanMeasureOfLocalityOutport_);
    addProperty(nrSubgroups_);
    addProperty(clusterCol_);
    addProperty(measureOfLocalityCol_);
}

void ClusterStatistics::process() {
    auto iCol = inport_.getData()->getIndexColumn();
    auto& indexCol = iCol->getTypedBuffer()->getRAMRepresentation()->getDataContainer();

    const auto clusters =
        inport_.getData()
            ->getColumn(clusterCol_.get())
            ->getBuffer()
            ->getRepresentation<BufferRAM>()
            ->dispatch<std::vector<int>, dispatching::filter::Scalars>([](auto buf) {
                auto& data = buf->getDataContainer();
                std::vector<int> dst(data.size(), 0.0f);
                std::transform(data.begin(), data.end(), dst.begin(),
                               [&](auto v) { return static_cast<int>(v); });
                return dst;
            });

    if (indexCol.size() != clusters.size()) {
        throw Exception("Unexpected dimension missmatch", IVW_CONTEXT);
    }
    const auto n = indexCol.size();

    // Create a map from cluster nr to the indices of all points in the cluster
    std::map<int, std::vector<uint32_t>> clusterNrToIndex = {};
    for (size_t i = 0; i < n; i++) {
        auto clusterNr = clusters[i];
        auto index = indexCol[i];

        if (clusterNrToIndex.empty()) {
            clusterNrToIndex.emplace(clusterNr, std::vector<uint32_t>{index});
        } else {
            auto it = clusterNrToIndex.find(clusterNr);

            if (it != clusterNrToIndex.end()) {
                it->second.push_back(index);
            } else {
                clusterNrToIndex.emplace(clusterNr, std::vector<uint32_t>{index});
            }
        }
    }

    // Get hole and particle charges for each subgroup
    std::vector<std::vector<float>> holeCharges = {};
    std::vector<std::vector<float>> particleCharges = {};
    const auto nrSubgroups = nrSubgroups_.get();
    for (size_t i = 0; i < nrSubgroups; i++) {
        auto holeColumnName = "Hole sg" + std::to_string(i + 1);
        auto particleColumnName = "Particle sg" + std::to_string(i + 1);

        auto holeCol = inport_.getData()->getColumn(holeColumnName);
        auto particleCol = inport_.getData()->getColumn(particleColumnName);

        if (holeCol == nullptr || particleCol == nullptr) {
            throw Exception(
                "Could not get hole or particle column, subgroup " + std::to_string(i + 1),
                IVW_CONTEXT);
        }

        holeCharges.push_back(
            holeCol->getBuffer()
                ->getRepresentation<BufferRAM>()
                ->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
                    auto& data = buf->getDataContainer();
                    std::vector<float> dst(data.size(), 0.0f);
                    std::transform(data.begin(), data.end(), dst.begin(),
                                   [&](auto v) { return static_cast<float>(v); });
                    return dst;
                }));

        particleCharges.push_back(
            particleCol->getBuffer()
                ->getRepresentation<BufferRAM>()
                ->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
                    auto& data = buf->getDataContainer();
                    std::vector<float> dst(data.size(), 0.0f);
                    std::transform(data.begin(), data.end(), dst.begin(),
                                   [&](auto v) { return static_cast<float>(v); });
                    return dst;
                }));
    }

    const auto measureOfLocalityCol = inport_.getData()->getColumn(measureOfLocalityCol_.get());

    if (measureOfLocalityCol == nullptr) {
        throw Exception("Could not get diff column", IVW_CONTEXT);
    }

    const auto measureOfLocalityData =
        measureOfLocalityCol->getBuffer()
            ->getRepresentation<BufferRAM>()
            ->dispatch<std::vector<float>, dispatching::filter::Scalars>([](auto buf) {
                auto& data = buf->getDataContainer();
                std::vector<float> dst(data.size(), 0.0f);
                std::transform(data.begin(), data.end(), dst.begin(),
                               [&](auto v) { return static_cast<float>(v); });
                return dst;
            });

    std::vector<int> clusterNr = {};
    std::vector<size_t> clusterSize = {};
    std::unordered_map<int, ClusterStatisticsStruct> subgroupToClusterStatistics_hole = {};
    std::unordered_map<int, ClusterStatisticsStruct> subgroupToClusterStatistics_particle = {};
    std::vector<float> meanOfMeasureOfLocaliesInClusters = {};

    for (auto&& c : clusterNrToIndex) {
        clusterNr.push_back(c.first);
        clusterSize.push_back(c.second.size());

        for (size_t i = 0; i < nrSubgroups; i++) {

            // Get hole and particle charges for this subgroup and cluster
            std::vector<float> holeChargesForCluster = {};
            std::vector<float> particleChargesForCluster = {};
            for (auto&& ind : c.second) {
                holeChargesForCluster.push_back(holeCharges[i][ind]);
                particleChargesForCluster.push_back(particleCharges[i][ind]);
            }

            // Min, max, mean and variance for hole and particle subgroups
            const auto minHole =
                *std::min_element(holeChargesForCluster.cbegin(), holeChargesForCluster.cend());
            const auto maxHole =
                *std::max_element(holeChargesForCluster.cbegin(), holeChargesForCluster.cend());
            const auto minParticle = *std::min_element(particleChargesForCluster.cbegin(),
                                                       particleChargesForCluster.cend());
            const auto maxParticle = *std::max_element(particleChargesForCluster.cbegin(),
                                                       particleChargesForCluster.cend());
            const auto meanHole = VectorStatistics::meanValue(holeChargesForCluster);
            const auto varianceHole = VectorStatistics::variance(holeChargesForCluster, meanHole);
            const auto meanParticle = VectorStatistics::meanValue(particleChargesForCluster);
            const auto varianceParticle =
                VectorStatistics::variance(particleChargesForCluster, meanParticle);

            if (subgroupToClusterStatistics_hole.empty() &&
                subgroupToClusterStatistics_particle.empty()) {

                auto statsHole = ClusterStatisticsStruct();
                statsHole.min = std::vector<float>{minHole};
                statsHole.max = std::vector<float>{maxHole};
                statsHole.diff = std::vector<float>{(maxHole - minHole)};
                statsHole.mean = std::vector<float>{meanHole};
                statsHole.variance = std::vector<float>{varianceHole};
                subgroupToClusterStatistics_hole.emplace(i, statsHole);

                auto statsParticle = ClusterStatisticsStruct();
                statsParticle.min = std::vector<float>{minParticle};
                statsParticle.max = std::vector<float>{maxParticle};
                statsParticle.diff = std::vector<float>{(maxParticle - minParticle)};
                statsParticle.mean = std::vector<float>{meanParticle};
                statsParticle.variance = std::vector<float>{varianceParticle};
                subgroupToClusterStatistics_particle.emplace(i, statsParticle);
            } else {
                auto itHole = subgroupToClusterStatistics_hole.find(i);
                auto itParticle = subgroupToClusterStatistics_particle.find(i);

                if (itHole != subgroupToClusterStatistics_hole.cend() &&
                    itParticle != subgroupToClusterStatistics_particle.cend()) {

                    itHole->second.min.push_back(minHole);
                    itHole->second.max.push_back(maxHole);
                    itHole->second.diff.push_back((maxHole - minHole));
                    itHole->second.mean.push_back(meanHole);
                    itHole->second.variance.push_back(varianceHole);
                    itParticle->second.min.push_back(minParticle);
                    itParticle->second.max.push_back(maxParticle);
                    itParticle->second.diff.push_back((maxParticle - minParticle));
                    itParticle->second.mean.push_back(meanParticle);
                    itParticle->second.variance.push_back(varianceParticle);
                } else {
                    auto statsHole = ClusterStatisticsStruct();
                    statsHole.min = std::vector<float>{minHole};
                    statsHole.max = std::vector<float>{maxHole};
                    statsHole.diff = std::vector<float>{(maxHole - minHole)};
                    statsHole.mean = std::vector<float>{meanHole};
                    statsHole.variance = std::vector<float>{varianceHole};
                    subgroupToClusterStatistics_hole.emplace(i, statsHole);

                    auto statsParticle = ClusterStatisticsStruct();
                    statsParticle.min = std::vector<float>{minParticle};
                    statsParticle.max = std::vector<float>{maxParticle};
                    statsParticle.diff = std::vector<float>{(maxParticle - minParticle)};
                    statsParticle.mean = std::vector<float>{meanParticle};
                    statsParticle.variance = std::vector<float>{varianceParticle};
                    subgroupToClusterStatistics_particle.emplace(i, statsParticle);
                }
            }
        }

        // Get measure of locality data for each cluster
        std::vector<float> allMeasureOfLocalityInCluster = {};
        for (auto&& ind : c.second) {
            allMeasureOfLocalityInCluster.push_back(measureOfLocalityData[ind]);
        }
        meanOfMeasureOfLocaliesInClusters.push_back(
            VectorStatistics::meanValue(allMeasureOfLocalityInCluster));
    }

    auto dataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));
    auto diffDataFrame =
        std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));
    auto meanDataFrame =
        std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));
    auto measureOfLocalityDataFrame =
        std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));

    dataFrame->addColumn("Cluster", clusterNr);
    dataFrame->addColumn("Cluster size", clusterSize);
    diffDataFrame->addColumn("Cluster", clusterNr);
    diffDataFrame->addColumn("Cluster size", clusterSize);
    meanDataFrame->addColumn("Cluster", clusterNr);
    meanDataFrame->addColumn("Cluster size", clusterSize);
    measureOfLocalityDataFrame->addColumn("Cluster", clusterNr);
    measureOfLocalityDataFrame->addColumn("Cluster size", clusterSize);
    measureOfLocalityDataFrame->addColumn("Mean of MeasureOfLocalities",
                                          meanOfMeasureOfLocaliesInClusters);

    for (size_t i = 0; i < nrSubgroups; i++) {
        dataFrame->addColumn("Min hole charge sg " + std::to_string(i + 1),
                             subgroupToClusterStatistics_hole[i].min);
        dataFrame->addColumn("Max hole charge sg " + std::to_string(i + 1),
                             subgroupToClusterStatistics_hole[i].max);
        dataFrame->addColumn("Min particle charge sg " + std::to_string(i + 1),
                             subgroupToClusterStatistics_particle[i].min);
        dataFrame->addColumn("Max particle charge sg " + std::to_string(i + 1),
                             subgroupToClusterStatistics_particle[i].max);
        diffDataFrame->addColumn("Diff hole charge sg " + std::to_string(i + 1),
                                 subgroupToClusterStatistics_hole[i].diff);
        diffDataFrame->addColumn("Diff particle charge sg " + std::to_string(i + 1),
                                 subgroupToClusterStatistics_particle[i].diff);
        meanDataFrame->addColumn("Mean hole charge sg " + std::to_string(i + 1),
                                 subgroupToClusterStatistics_hole[i].mean);
        meanDataFrame->addColumn("Variance hole charge sg " + std::to_string(i + 1),
                                 subgroupToClusterStatistics_hole[i].variance);
        meanDataFrame->addColumn("Mean particle charge sg " + std::to_string(i + 1),
                                 subgroupToClusterStatistics_particle[i].mean);
        meanDataFrame->addColumn("Variance particle charge sg " + std::to_string(i + 1),
                                 subgroupToClusterStatistics_particle[i].variance);
    }

    outport_.setData(dataFrame);
    diffOutport_.setData(diffDataFrame);
    meanOutport_.setData(meanDataFrame);
    meanMeasureOfLocalityOutport_.setData(measureOfLocalityDataFrame);
}

}  // namespace inviwo
