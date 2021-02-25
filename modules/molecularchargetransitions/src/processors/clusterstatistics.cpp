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
const ProcessorInfo ClusterStatistics::getProcessorInfo() const { return processorInfo_; }

ClusterStatistics::ClusterStatistics()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , diffOutport_("diffOutport")
    , meanOutport_("meanOutport")
    , nrSubgroups_("nrSubgroups", "Nr of subgroups", 2, 1, 10, 1) {

    addPort(inport_);
    addPort(outport_);
    addPort(diffOutport_);
    addPort(meanOutport_);
    addProperty(nrSubgroups_);
}

float meanValue(const std::vector<float>& charges) {
    const auto sum = std::accumulate(charges.begin(), charges.end(), 0.0f);
    return sum / charges.size();
}

float standardDeviation(const std::vector<float>& charges, const float& mean) {
    const auto sqSum = std::inner_product(charges.begin(), charges.end(), charges.begin(), 0.0f);
    return std::sqrt(sqSum / charges.size() - mean * mean);
}

void ClusterStatistics::process() {
    auto iCol = inport_.getData()->getIndexColumn();
    auto& indexCol = iCol->getTypedBuffer()->getRAMRepresentation()->getDataContainer();

    const auto clusters =
        inport_.getData()
            ->getColumn("Cluster")
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
            auto& it = clusterNrToIndex.find(clusterNr);

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

    std::vector<int> clusterNr = {};
    std::vector<size_t> clusterSize = {};
    // TODO: put these into some shared data structure...
    std::unordered_map<int, std::vector<float>> subgroupToHoleCharges_min;
    std::unordered_map<int, std::vector<float>> subgroupToHoleCharges_max;
    std::unordered_map<int, std::vector<float>> subgroupToHoleCharges_diff;
    std::unordered_map<int, std::vector<float>> subgroupToHoleCharges_stdev;
    std::unordered_map<int, std::vector<float>> subgroupToHoleCharges_mean;
    std::unordered_map<int, std::vector<float>> subgroupToParticleCharges_min;
    std::unordered_map<int, std::vector<float>> subgroupToParticleCharges_max;
    std::unordered_map<int, std::vector<float>> subgroupToParticleCharges_diff;
    std::unordered_map<int, std::vector<float>> subgroupToParticleCharges_stdev;
    std::unordered_map<int, std::vector<float>> subgroupToParticleCharges_mean;

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

            // Min, max, mean and standard deviation for hole and particle
            const auto minHole =
                std::min_element(holeChargesForCluster.cbegin(), holeChargesForCluster.cend());
            const auto maxHole =
                std::max_element(holeChargesForCluster.cbegin(), holeChargesForCluster.cend());
            const auto minParticle = std::min_element(particleChargesForCluster.cbegin(),
                                                      particleChargesForCluster.cend());
            const auto maxParticle = std::max_element(particleChargesForCluster.cbegin(),
                                                      particleChargesForCluster.cend());
            const auto meanHole = meanValue(holeChargesForCluster);
            const auto stDevHole = standardDeviation(holeChargesForCluster, meanHole);
            const auto meanParticle = meanValue(particleChargesForCluster);
            const auto stDevParticle = standardDeviation(particleChargesForCluster, meanParticle);

            if (subgroupToHoleCharges_min.empty()) {
                subgroupToHoleCharges_min.emplace(i, std::vector<float>{*minHole});
                subgroupToHoleCharges_max.emplace(i, std::vector<float>{*maxHole});
                subgroupToHoleCharges_diff.emplace(i, std::vector<float>{(*maxHole - *minHole)});
                subgroupToHoleCharges_mean.emplace(i, std::vector<float>{meanHole});
                subgroupToHoleCharges_stdev.emplace(i, std::vector<float>{stDevHole});
                subgroupToParticleCharges_min.emplace(i, std::vector<float>{*minParticle});
                subgroupToParticleCharges_max.emplace(i, std::vector<float>{*maxParticle});
                subgroupToParticleCharges_diff.emplace(
                    i, std::vector<float>{(*maxParticle - *minParticle)});
                subgroupToParticleCharges_mean.emplace(i, std::vector<float>{meanParticle});
                subgroupToParticleCharges_stdev.emplace(i, std::vector<float>{stDevParticle});
            } else {
                auto& it = subgroupToHoleCharges_min.find(i);

                if (it != subgroupToHoleCharges_min.cend()) {
                    it->second.push_back(*minHole);

                    subgroupToHoleCharges_max.find(i)->second.push_back(*maxHole);
                    subgroupToHoleCharges_diff.find(i)->second.push_back((*maxHole - *minHole));
                    subgroupToHoleCharges_mean.find(i)->second.push_back(meanHole);
                    subgroupToHoleCharges_stdev.find(i)->second.push_back(stDevHole);
                    subgroupToParticleCharges_min.find(i)->second.push_back(*minParticle);
                    subgroupToParticleCharges_max.find(i)->second.push_back(*maxParticle);
                    subgroupToParticleCharges_diff.find(i)->second.push_back(
                        (*maxParticle - *minParticle));
                    subgroupToParticleCharges_mean.find(i)->second.push_back(meanParticle);
                    subgroupToParticleCharges_stdev.find(i)->second.push_back(stDevParticle);
                } else {
                    subgroupToHoleCharges_min.emplace(i, std::vector<float>{*minHole});
                    subgroupToHoleCharges_max.emplace(i, std::vector<float>{*maxHole});
                    subgroupToHoleCharges_diff.emplace(i,
                                                       std::vector<float>{(*maxHole - *minHole)});
                    subgroupToHoleCharges_mean.emplace(i, std::vector<float>{meanHole});
                    subgroupToHoleCharges_stdev.emplace(i, std::vector<float>{stDevHole});
                    subgroupToParticleCharges_min.emplace(i, std::vector<float>{*minParticle});
                    subgroupToParticleCharges_max.emplace(i, std::vector<float>{*maxParticle});
                    subgroupToParticleCharges_diff.emplace(
                        i, std::vector<float>{(*maxParticle - *minParticle)});
                    subgroupToParticleCharges_mean.emplace(i, std::vector<float>{meanParticle});
                    subgroupToParticleCharges_stdev.emplace(i, std::vector<float>{stDevParticle});
                }
            }
        }
    }

    auto dataFrame = std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));
    auto diffDataFrame =
        std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));
    auto meanDataFrame =
        std::make_shared<DataFrame>(static_cast<glm::u32>(clusterNrToIndex.size()));

    dataFrame->addColumn("Cluster", clusterNr);
    dataFrame->addColumn("Cluster size", clusterSize);
    diffDataFrame->addColumn("Cluster", clusterNr);
    diffDataFrame->addColumn("Cluster size", clusterSize);
    meanDataFrame->addColumn("Cluster", clusterNr);
    meanDataFrame->addColumn("Cluster size", clusterSize);

    for (size_t i = 0; i < nrSubgroups; i++) {
        dataFrame->addColumn("Min hole charge sg " + std::to_string(i + 1),
                             subgroupToHoleCharges_min[i]);
        dataFrame->addColumn("Max hole charge sg " + std::to_string(i + 1),
                             subgroupToHoleCharges_max[i]);
        dataFrame->addColumn("Min particle charge sg " + std::to_string(i + 1),
                             subgroupToParticleCharges_min[i]);
        dataFrame->addColumn("Max particle charge sg " + std::to_string(i + 1),
                             subgroupToParticleCharges_max[i]);

        diffDataFrame->addColumn("Diff hole charge sg " + std::to_string(i + 1),
                                 subgroupToHoleCharges_diff[i]);
        diffDataFrame->addColumn("Diff particle charge sg " + std::to_string(i + 1),
                                 subgroupToParticleCharges_diff[i]);

        meanDataFrame->addColumn("Mean hole charge sg " + std::to_string(i + 1),
                                 subgroupToHoleCharges_mean[i]);
        meanDataFrame->addColumn("Stdev hole charge sg " + std::to_string(i + 1),
                                 subgroupToHoleCharges_stdev[i]);
        meanDataFrame->addColumn("Mean particle charge sg " + std::to_string(i + 1),
                                 subgroupToParticleCharges_mean[i]);
        meanDataFrame->addColumn("Stdev particle charge sg " + std::to_string(i + 1),
                                 subgroupToParticleCharges_stdev[i]);
    }

    outport_.setData(dataFrame);
    diffOutport_.setData(diffDataFrame);
    meanOutport_.setData(meanDataFrame);
}

}  // namespace inviwo
