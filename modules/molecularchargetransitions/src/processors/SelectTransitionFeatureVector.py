# Name: SelectTransitionFeatureVector 

import inviwopy as ivw
import ivwdataframe as df

import numpy as np

class SelectTransitionFeatureVector(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)
        self.outport = df.DataFrameOutport("outport")
        self.addOutport(self.outport)

        self.featureVectorSelection = ivw.properties.OptionPropertyString(
            "feature_vector", "Feature vector", [ivw.properties.StringOption("holeAndParticle", "Hole and particle charges", "holeAndParticle"), 
                                                 ivw.properties.StringOption("chargeTransferMatrix", "Full charge transfer matrix", "chargeTransferMatrix"),
                                                 ivw.properties.StringOption("trace", "Trace of matrix", "trace"),
                                                 ivw.properties.StringOption("bottleneck", "Bottleneck", "bottleneck"),
                                                 ivw.properties.StringOption("columnRange", "Column range", "columnRange")])
        self.addProperty(self.featureVectorSelection)

        self.colRangeStart = ivw.properties.IntProperty(
            "column_range_start", "Start, column range", 0, 0, 1000, 1)
        self.addProperty(self.colRangeStart)
        self.colRangeEnd = ivw.properties.IntProperty(
            "column_range_end", "End, column range", 10, 0, 1000, 1)
        self.addProperty(self.colRangeEnd)

        self.useOscStrength = ivw.properties.BoolProperty(
            "use_osc_strength", "Use oscillator strength", False)
        self.addProperty(self.useOscStrength)
        self.useRotStrength = ivw.properties.BoolProperty(
            "use_rot_strenght", "Use rotatory strength", False)
        self.addProperty(self.useRotStrength)
        self.useEnergy = ivw.properties.BoolProperty(
            "use_energy", "Use energy", False)
        self.addProperty(self.useEnergy)
        self.useTrace = ivw.properties.BoolProperty(
            "use_trace", "Use trace", False)
        self.addProperty(self.useTrace)

        self.featureVectorName = ivw.properties.StringProperty("featureVectorName", "Name of feature vector", "TranFV")
        self.addProperty(self.featureVectorName)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.SelectTransitionFeatureVector", 
    		displayName = "Select Transition Feature Vector",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return SelectTransitionFeatureVector.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        inputDataFrame = self.dataFrame.getData()

        # Get feature vector from data frame
        X = []
        for i in range(0, inputDataFrame.rows):
            row = []
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()

                # Use hole and particle subgroup charges
                if self.featureVectorSelection.value == "holeAndParticle":
                    if ("hole" in header) or ("particle" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Use charge transfer matrix
                elif self.featureVectorSelection.value == "chargeTransferMatrix":
                    if ("transfer" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Use trace of matrix
                elif self.featureVectorSelection.value == "trace":
                    if ("transfer" in header and header[-1] == header[-2]):
                        row.append(inputDataFrame.column(j).get(i))

                # bottleneck
                elif self.featureVectorSelection.value == "bottleneck":
                    if ("h0" in header or "h2" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Use column range
                elif self.featureVectorSelection.value == "columnRange":
                    if (j >= self.colRangeStart.value and j <= self.colRangeEnd.value):
                        row.append(inputDataFrame.column(j).get(i))

                # Add oscillatory strength to feature vector
                if self.useOscStrength.value == True : 
                    if ("osc" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Add rotatory strength to feature vector
                if self.useRotStrength.value == True :
                    if ("rot" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Add energy to feature vector
                if self.useEnergy.value == True :
                    if ("energy" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Add trace to feature vector
                if self.useTrace.value == True :
                    if ("transfer" in header and header[-1] == header[-2]):
                        row.append(inputDataFrame.column(j).get(i))
            
            X.append(row)

        X = np.array(X)
        X_transp = np.transpose(X)
        print("Length of feature vector: " + str(X_transp.shape[0]))

        # Put each dimension (column) of the resulting feature vector into the output data frame
        outputDataframe = df.DataFrame()
        for i in range(0, X_transp.shape[0]):
            outputDataframe.addFloatColumn(self.featureVectorName.value + str(i+1), X_transp[i,:])

        outputDataframe.updateIndex()

        self.outport.setData(outputDataframe)

