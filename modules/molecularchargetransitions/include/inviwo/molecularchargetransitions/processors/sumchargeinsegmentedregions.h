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

#pragma once

#include <inviwo/molecularchargetransitions/molecularchargetransitionsmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/util/volumeramutils.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/util/filesystem.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>

namespace inviwo {

/** \docpage{org.inviwo.SumChargeInSegmentedRegions, Sum Charge In Segmented Regions}
 * ![](org.inviwo.SumChargeInSegmentedRegions.png?classIdentifier=org.inviwo.SumChargeInSegmentedRegions)
 *
 * Sums up the values (charge) in a volume based on a segmentation of that volume. Also adds these
 * regions together based on a subgroup file provided.
 *
 * ### Inports
 *   * __segmentation__ Segmentation of the volume.
 *   * __volumeValues__ The volume containing values (charges) that should be accumulated.
 *
 * ### Outports
 *   * __chargePerRegion__ Summed up value (charge) per segmented region.
 *   * __chargePerSubgroup__ Summed up value (charge) per subgroup, which is multiple regions.
 *
 * ### Properties
 *   * __fileLocation__ Path to a file stating which regions belong to each subgroup.
 */
class IVW_MODULE_MOLECULARCHARGETRANSITIONS_API SumChargeInSegmentedRegions : public Processor {
public:
    SumChargeInSegmentedRegions();
    virtual ~SumChargeInSegmentedRegions() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VolumeInport segmentation_;
    VolumeInport volumeValues_;
    DataFrameOutport chargePerRegion_;
    DataFrameOutport chargePerSubgroup_;
    FileProperty fileLocation_;
};

}  // namespace inviwo
