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
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/dataframe/properties/columnoptionproperty.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/molecularchargetransitions/algorithm/statistics.h>

namespace inviwo {

/** \docpage{org.inviwo.ClusterStatistics, Cluster Statistics}
 * ![](org.inviwo.ClusterStatistics.png?classIdentifier=org.inviwo.ClusterStatistics)
 *
 * Processor to calculate some statistics for an ensemble of electronic transitions.
 *
 * ### Inports
 *   * __inport__   Dataframe containing cluster id, hole charges, particle charges and measure of
 * locality value for each ensemble member.
 *
 * ### Outports
 *   * __outport__ Min and max particle and hole charges for each cluster??.
 *   * __diffOutport__ Difference between min and max within each cluster??.
 *   * __meanOutport__ Mean and variance for each cluster.
 *   * __meanMeasureOfLocalityOutport__ Mean of measure of locality for each cluster.
 *
 * ### Properties
 *   * __nrSubgroups__ How many subgroups each member in the ensemble has.
 *   * __clusterCol__ Selecting which column contains the cluster id.
 *   * __measureOfLocalityCol__ Selecting which column contains measure of locality value.
 */
class IVW_MODULE_MOLECULARCHARGETRANSITIONS_API ClusterStatistics : public Processor {
public:
    ClusterStatistics();
    virtual ~ClusterStatistics() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    struct ClusterStatisticsStruct {
        std::vector<float> min;
        std::vector<float> max;
        std::vector<float> diff;
        std::vector<float> mean;
        std::vector<float> stdev;
        std::vector<float> variance;
    };

    DataFrameInport inport_;
    DataFrameOutport outport_;
    DataFrameOutport diffOutport_;
    DataFrameOutport meanOutport_;
    DataFrameOutport meanMeasureOfLocalityOutport_;
    IntProperty nrSubgroups_;
    ColumnOptionProperty clusterCol_;
    ColumnOptionProperty measureOfLocalityCol_;
};

}  // namespace inviwo
