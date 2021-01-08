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
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <vector>

namespace inviwo {

	/** \docpage{org.inviwo.ComputeChargeTransfer, Compute Charge Transfer}
	 * ![](org.inviwo.ComputeChargeTransfer.png?classIdentifier=org.inviwo.ComputeChargeTransfer)
	 *
	 * Processor to calculate the charge transfer matrix, given the hole and particle charges for each subgroup.
	 * It also calculates the difference in charge from hole to particle charge for each subgroup.
	 *
	 * ### Inports
	 *   * __holeCharges__      The hole charges for each subgroup (column name charge_sg).
	 *   * __particleCharges__  The particle charges for each subgroup (column name charge_sg).
	 *                          Must be same length as holeCharges_.
	 *
	 * ### Outports
	 *   * __chargeDifference__ Difference in charge from hole to particle ("particle - hole") for each subgroup.
	 *   * __chargeTransfer__   Charge transfer matrix.
	 *
	 */
	class IVW_MODULE_MOLECULARCHARGETRANSITIONS_API ComputeChargeTransfer : public Processor {
	public:
		ComputeChargeTransfer();
		virtual ~ComputeChargeTransfer() = default;

		virtual void process() override;

		virtual const ProcessorInfo getProcessorInfo() const override;
		static const ProcessorInfo processorInfo_;

	private:
		DataFrameInport holeCharges_;
		DataFrameInport particleCharges_;
		DataFrameOutport chargeDifference_;
		DataFrameOutport chargeTransfer_;
	};

}  // namespace inviwo
